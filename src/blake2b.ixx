module;
#include <array>
#include <charconv>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <system_error>


export module hash.blake2b;
import piku.assert;
import piku.types;

using namespace std::string_view_literals;
using namespace piku;

namespace hash
{

    static inline u64  rotr64(const u64 w, const unsigned c) noexcept { return (w >> c) | (w << (64 - c)); }
    static inline void store64(void *dst, u64 w) noexcept { memcpy(dst, &w, sizeof w); }
    static inline u64  load64(const void *src) noexcept
    {
        u64 w;
        memcpy(&w, src, sizeof(w));
        return w;
    }

    /* prevents compiler optimizing out memset() */
    static inline void secure_zero_memory(void *v, u64 n)
    {
        static void *(*const volatile memset_v)(void *, int, u64) = &memset;
        memset_v(v, 0, n);
    }
    enum blake2b_constant : u32
    {
        BLAKE2B_BLOCKBYTES = 128,
        BLAKE2B_OUTBYTES   = 64,
        BLAKE2B_KEYBYTES   = BLAKE2B_OUTBYTES,

        BLAKE2_PRINTABLE_BYTES     = (BLAKE2B_OUTBYTES * 2),
        BLAKE2_PRINTABLE_KEY_BYTES = (BLAKE2B_KEYBYTES * 2),
    };


    export class blake2_key final
    {
    public:
        blake2_key() = default;
        blake2_key(const std::initializer_list<u8> &digits)
        {
            size_t len = digits.size() <= binary.size() ? digits.size() : binary.size();
            std::copy_n(digits.begin(), len, binary.begin());
            valid = true;
        }
        [[nodiscard]] bool from_hex(std::string_view hexkey)
        {
            bool                   has_key = !hexkey.empty();
            std::from_chars_result res{.ec = std::errc::invalid_argument};
            for (u8 &i : binary)
            {
                if (hexkey.size() == 0)
                    break;
                size_t len = std::min(as<size_t>(2), hexkey.size());
                res        = std::from_chars(hexkey.data(), hexkey.data() + len, i, 16);

                hexkey.remove_prefix(len);
                keylen++;
            }
            valid = has_key && (res.ec == std::errc());
            return valid;
        }

        std::string to_string(const Uppercase uppercase = Uppercase::No) const noexcept
        {
            constexpr static std::array fmt_array{"{:02x}"sv, "{:02X}"sv};
            const int                   index = uppercase == Uppercase::No ? 0 : 1;
            std::string                 ret;
            ret.reserve(BLAKE2_PRINTABLE_KEY_BYTES);

            for (size_t i = 0; i < binary.size(); i += 8)
            {
                ret.append(std::vformat(std::format("{0}{0}{0}{0}{0}{0}{0}{0}", fmt_array[index]),
                                        std::make_format_args(binary[i + 0],
                                                              binary[i + 1],
                                                              binary[i + 2],
                                                              binary[i + 3],
                                                              binary[i + 4],
                                                              binary[i + 5],
                                                              binary[i + 6],
                                                              binary[i + 7])));
            }
            return ret;
        }
        bool operator==(const blake2_key &that) const noexcept { return binary == that.binary; }

        std::array<u8, BLAKE2B_KEYBYTES> binary{0};
        auto                             begin() const noexcept { return binary.begin(); }
        auto                             end() const noexcept { return binary.end(); }
        auto                             data() const noexcept { return binary.data(); }

        bool valid{false};
        u8   keylen{0};
        u8   length() const noexcept { return keylen; }
        bool is_valid() const noexcept { return valid; }
    };


    export class [[nodiscard("You are not using your hash digest.")]] blake2_digest final
    {
    public:
        [[nodiscard]] bool from_hex(std::string_view hexkey)
        {
            std::from_chars_result res;
            for (u8 &i : binary)
            {
                if (hexkey.size() == 0)
                    break;
                size_t len = std::min(as<size_t>(2), hexkey.size());
                res        = std::from_chars(hexkey.data(), hexkey.data() + len, i, 16);

                hexkey.remove_prefix(len);
            }
            return res.ec == std::errc();
        }

        [[nodiscard("You are not using your hash digest.")]] std::string to_string(const Uppercase uppercase
                                                                                   = Uppercase::No) const noexcept
        {
            constexpr static std::array fmt_array{"{:02x}"sv, "{:02X}"sv};
            const int                   index = uppercase == Uppercase::No ? 0 : 1;
            std::string                 ret;
            ret.reserve(BLAKE2_PRINTABLE_BYTES);

            for (size_t i = 0; i < binary.size(); i += 8)
            {
                ret.append(std::vformat(std::format("{0}{0}{0}{0}{0}{0}{0}{0}", fmt_array[index]),
                                        std::make_format_args(binary[i + 0],
                                                              binary[i + 1],
                                                              binary[i + 2],
                                                              binary[i + 3],
                                                              binary[i + 4],
                                                              binary[i + 5],
                                                              binary[i + 6],
                                                              binary[i + 7])));
            }
            return ret;
        }
        bool operator==(const blake2_digest &that) const noexcept { return binary == that.binary; }

        std::array<u8, BLAKE2B_OUTBYTES> binary{};
    };


    export class blake2 final
    {
        // https://www.rfc-editor.org/rfc/rfc7693.txt
        struct blake2b_ctx
        {
            std::array<u8, BLAKE2B_BLOCKBYTES> input_block{0};
            std::array<u64, 8>                 state{0};
            std::array<u64, 2>                 total_bytes{0};
            size_t                             input_block_ptr{0};
            size_t                             digest_len{0};
        };

        blake2b_ctx ctx;


    public:
        blake2() = default;

        void init() noexcept { init({}); }
        void init(const blake2_key &key) noexcept
        {
            const u8 keylen = key.is_valid() ? key.length() : 0u;

            for (int i = 0; i < 8; i++)   // state, "param block"
                ctx.state[i] = blake2b_IV[i];

            ctx.state[0] ^= 0x01010000 ^ (keylen << 8) ^ BLAKE2B_OUTBYTES;

            ctx.total_bytes[0]  = 0;   // input count low word
            ctx.total_bytes[1]  = 0;   // input count high word
            ctx.input_block_ptr = 0;   // pointer within buffer
            ctx.digest_len      = BLAKE2B_OUTBYTES;

            for (int i = keylen; i < 128; i++)   // zero input block
                ctx.input_block[i] = 0;

            if (key.is_valid())
            {
                std::array<u8, BLAKE2B_BLOCKBYTES> block;
                block.fill(0);
                std::copy_n(key.begin(), key.length(), block.begin());

                update(block);
                ctx.input_block_ptr = 128;   // at the end
                secure_zero_memory(block.data(), block.size());
            }
        }


#define B2B_G(a, b, c, d, x, y)                                                                                        \
    {                                                                                                                  \
        v[a] = v[a] + v[b] + x;                                                                                        \
        v[d] = rotr64(v[d] ^ v[a], 32);                                                                                \
        v[c] = v[c] + v[d];                                                                                            \
        v[b] = rotr64(v[b] ^ v[c], 24);                                                                                \
        v[a] = v[a] + v[b] + y;                                                                                        \
        v[d] = rotr64(v[d] ^ v[a], 16);                                                                                \
        v[c] = v[c] + v[d];                                                                                            \
        v[b] = rotr64(v[b] ^ v[c], 63);                                                                                \
    }

        void compress(int last) noexcept
        {
            u64 v[16]{};
            u64 m[16]{};

            for (int i = 0; i < 8; i++)
            {
                v[i]     = ctx.state[i];
                v[i + 8] = blake2b_IV[i];
            }

            v[12] ^= ctx.total_bytes[0];   // low 64 bits of offset
            v[13] ^= ctx.total_bytes[1];   // high 64 bits
            if (last)                      // last block flag set ?
                v[14] = ~v[14];

            for (int i = 0; i < 16; i++)   // get little-endian words
                m[i] = load64(&ctx.input_block[8 * i]);


            for (int i = 0; i < 12; i++)
            {
                B2B_G(0, 4, 8, 12, m[blake2b_sigma[i][0]], m[blake2b_sigma[i][1]]);
                B2B_G(1, 5, 9, 13, m[blake2b_sigma[i][2]], m[blake2b_sigma[i][3]]);
                B2B_G(2, 6, 10, 14, m[blake2b_sigma[i][4]], m[blake2b_sigma[i][5]]);
                B2B_G(3, 7, 11, 15, m[blake2b_sigma[i][6]], m[blake2b_sigma[i][7]]);
                B2B_G(0, 5, 10, 15, m[blake2b_sigma[i][8]], m[blake2b_sigma[i][9]]);
                B2B_G(1, 6, 11, 12, m[blake2b_sigma[i][10]], m[blake2b_sigma[i][11]]);
                B2B_G(2, 7, 8, 13, m[blake2b_sigma[i][12]], m[blake2b_sigma[i][13]]);
                B2B_G(3, 4, 9, 14, m[blake2b_sigma[i][14]], m[blake2b_sigma[i][15]]);
            }

            for (int i = 0; i < 8; ++i)
                ctx.state[i] ^= v[i] ^ v[i + 8];
        }

        void update(std::string_view input_sv) noexcept { generic_update<const char>(input_sv); }
        void update(std::span<u8> input_span) noexcept { generic_update<u8>(input_span); }


        template <typename T> void generic_update(std::span<T> input_block) noexcept
        {

            for (size_t i = 0; i < input_block.size(); i++)
            {
                if (ctx.input_block_ptr == 128)
                {                                                   // buffer full ?
                    ctx.total_bytes[0] += ctx.input_block_ptr;      // add counters
                    if (ctx.total_bytes[0] < ctx.input_block_ptr)   // carry overflow ?
                        ctx.total_bytes[1]++;                       // high word
                    compress(0);                                    // compress (not last)
                    ctx.input_block_ptr = 0;                        // counter to zero
                }
                ctx.input_block[ctx.input_block_ptr++] = input_block[i];
            }
        }

        blake2_digest final() noexcept
        {
            blake2_digest digest;
            size_t        i;

            ctx.total_bytes[0] += ctx.input_block_ptr;      // mark last block offset
            if (ctx.total_bytes[0] < ctx.input_block_ptr)   // carry overflow
                ctx.total_bytes[1]++;                       // high word

            while (ctx.input_block_ptr < 128)   // fill up with zeros
                ctx.input_block[ctx.input_block_ptr++] = 0;

            compress(1);   // final block flag = 1


            for (i = 0; i < ctx.digest_len; i++)
                digest.binary[i] = (u8)(ctx.state[i >> 3] >> (8 * (i & 7))) & 0xFF;

            return digest;
        }


        blake2_digest hash(const blake2_key &key, std::span<u8> input)
        {
            blake2b_ctx ctx;

            init(key);
            update(input);
            return final();
        }


    private:
        static constexpr u64 blake2b_IV[8] = {0x6a09e667f3bcc908ULL,
                                              0xbb67ae8584caa73bULL,
                                              0x3c6ef372fe94f82bULL,
                                              0xa54ff53a5f1d36f1ULL,
                                              0x510e527fade682d1ULL,
                                              0x9b05688c2b3e6c1fULL,
                                              0x1f83d9abfb41bd6bULL,
                                              0x5be0cd19137e2179ULL};

        static constexpr u8 blake2b_sigma[12][16] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                                                     {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
                                                     {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
                                                     {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
                                                     {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
                                                     {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
                                                     {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
                                                     {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
                                                     {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
                                                     {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
                                                     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                                                     {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}};
    };

    constexpr auto blake2size = sizeof(blake2);


}   // namespace hash
