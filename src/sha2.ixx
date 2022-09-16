module;
#include <array>
#include <bit>
#include <cstdint>
#include <format>
#include <string>
#include <span>

export module hash.sha2;

namespace hash
{

    constexpr uint32_t SHA256_CHUNK_SIZE_IN_BITS = 512;
    constexpr uint32_t SHA256_BLOCK_SIZE         = SHA256_CHUNK_SIZE_IN_BITS / 8;
    constexpr uint32_t SHA256_BINARY_BLOCK_SIZE  = SHA256_BLOCK_SIZE / 8;
    constexpr uint32_t SHA256_ROUNDS             = 64;


    export enum Uppercase : uint8_t {
        No,
        Yes,
    };


    template <typename T, typename U> T load_bigendian(U const &bytes)
    {
        T ret{};
        std::memcpy(&ret, bytes, sizeof(T));
        return std::byteswap(ret);
    }


    export class [[nodiscard("You are getting SHA256 digest but not using it.")]] sha256digest final
    {
    public:
        // clang-format off
    [[nodiscard("You are getting the SHA256 digest string but not using it.")]]
    std::string to_string(Uppercase uppercase = Uppercase::No) const noexcept
        // clang-format on
        {

            return std::vformat(
                std::format("{0}{0}{0}{0}{0}{0}{0}{0}", uppercase == Uppercase::No ? "{:08x}" : "{:08X}"),
                std::make_format_args(
                    binary[0], binary[1], binary[2], binary[3], binary[4], binary[5], binary[6], binary[7]));
        }

        uint32_t operator[](int index) const noexcept { return binary[static_cast<uint64_t>(index)]; }

        bool operator==(const sha256digest &that) const noexcept { return binary == that.binary; }

        std::array<uint32_t, SHA256_BINARY_BLOCK_SIZE> binary{};
    };

    static_assert(sizeof(sha256digest) == 32);


    export class sha256 final
    {
    public:
        sha256() { reset(); }
        void reset() noexcept
        {
            m_state  = {0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19};
            m_block  = {};
            m_bitlen = 0ULL;
            m_blockindex = 0ULL;
        }
        void update(std::span<const char> const data) noexcept { generic_update<const char>(data); }
        void update(std::span<uint8_t> const data) noexcept { generic_update<uint8_t>(data); }

        sha256digest finalize() noexcept
        {
            sha256digest ret;

            pad();
            ret.binary = m_state;
            reset();
            return ret;
        }

    private:
        template <typename T> void generic_update(const std::span<T> data) noexcept
        {
            if (data.empty())
                return;

            for (const auto &i : data)
            {
                m_block[m_blockindex++] = static_cast<uint8_t>(i);
                if (m_blockindex == SHA256_BLOCK_SIZE)
                {
                    transform();

                    m_bitlen += SHA256_CHUNK_SIZE_IN_BITS;
                    m_blockindex = 0;
                }
            }
        }
        static uint32_t choose(uint32_t e, uint32_t f, uint32_t g) noexcept { return (e & f) ^ (~e & g); }
        static uint32_t majority(uint32_t a, uint32_t b, uint32_t c) noexcept { return (a & b) ^ (a & c) ^ (b & c); }

        static uint32_t sig0(uint32_t x) noexcept { return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3); }
        static uint32_t sig1(uint32_t x) noexcept { return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10); }

        void transform()   // Process the message in successive 512-bit chunks
        {
            uint32_t maj{}, S0{}, ch{}, S1{}, temp1{}, temp2{}, w[SHA256_ROUNDS]{0};
            std::array<uint32_t, SHA256_BINARY_BLOCK_SIZE> state;

            // copy chunk into first 16 words w[0..15] of the message schedule array
            for (uint32_t i = 0, j = 0; i < 16; i++, j += 4)
                w[i] = load_bigendian<uint32_t>(&m_block[j]);

            for (uint32_t i = 16; i < SHA256_ROUNDS; i++)
            {
                uint32_t s0 = sha256::sig0(w[i - 15]);
                uint32_t s1 = sha256::sig1(w[i - 2]);
                w[i]        = w[i - 16] + s0 + w[i - 7] + s1;
            }
            // Initialize working variables to current hash value
            state = m_state;

            //  Compression function main loop
            for (uint32_t i = 0; i < SHA256_ROUNDS; i++)
            {

                S1    = std::rotr(state[4], 6) ^ std::rotr(state[4], 11) ^ std::rotr(state[4], 25);
                ch    = choose(state[4], state[5], state[6]);
                temp1 = state[7] + S1 + ch + K[i] + w[i];
                S0    = std::rotr(state[0], 2) ^ std::rotr(state[0], 13) ^ std::rotr(state[0], 22);
                maj   = sha256::majority(state[0], state[1], state[2]);
                temp2 = S0 + maj;

                state[7] = state[6];           // h = g
                state[6] = state[5];           // g = f
                state[5] = state[4];           // f = e
                state[4] = state[3] + temp1;   // d + temp1
                state[3] = state[2];           // d = c
                state[2] = state[1];           // c = b
                state[1] = state[0];           // b = a
                state[0] = temp1 + temp2;      // a = temp1 + temp2
            }

            // Add the compressed chunk to the current hash value
            for (uint32_t i = 0; i < 8; i++)
                m_state[i] += state[i];
        }

        void pad()
        {
            uint64_t i = m_blockindex;

            if (m_blockindex < (SHA256_BLOCK_SIZE - 8))
            {
                m_block[i++] = 0x80;
                while (i < (SHA256_BLOCK_SIZE - 8))
                    m_block[i++] = 0x00;
            }
            else
            {
                m_block[i++] = 0x80;
                while (i < (SHA256_BLOCK_SIZE - 8))
                    m_block[i++] = 0x00;

                std::fill_n(m_block.begin(), SHA256_BLOCK_SIZE - 8, (uint8_t)0);
                transform();
            }

            m_bitlen += m_blockindex * 8;
            m_block[63] = static_cast<uint8_t>(m_bitlen >> 0);
            m_block[62] = static_cast<uint8_t>(m_bitlen >> 8);
            m_block[61] = static_cast<uint8_t>(m_bitlen >> 16);
            m_block[60] = static_cast<uint8_t>(m_bitlen >> 24);
            m_block[59] = static_cast<uint8_t>(m_bitlen >> 32);
            m_block[58] = static_cast<uint8_t>(m_bitlen >> 40);
            m_block[57] = static_cast<uint8_t>(m_bitlen >> 48);
            m_block[56] = static_cast<uint8_t>(m_bitlen >> 56);

            transform();
        }

        std::array<uint8_t, SHA256_BLOCK_SIZE>         m_block;
        std::array<uint32_t, SHA256_BINARY_BLOCK_SIZE> m_state;
        uint64_t                                       m_bitlen;
        uint32_t                                       m_blockindex;

        static constexpr std::array<uint32_t, 64> K
            = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
               0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
               0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
               0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
               0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
               0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
               0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
               0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    };

    static_assert(sizeof(sha256) == 112);


    // SHA512

    constexpr uint32_t SHA512_CHUNK_SIZE_IN_BITS = 1024;
    constexpr uint32_t SHA512_BLOCK_SIZE         = SHA512_CHUNK_SIZE_IN_BITS / 8;
    constexpr uint32_t SHA512_ROUNDS             = 80;

    export class [[nodiscard("You are getting SHA512 digest but not using it.")]] sha512digest final
    {
    public:
        // clang-format off
    [[nodiscard("You are getting the SHA512 digest string but not using it.")]]
    std::string to_string(Uppercase uppercase = Uppercase::No) const noexcept
        // clang-format on
        {

            return std::vformat(
                std::format("{0}{0}{0}{0}{0}{0}{0}{0}", uppercase == Uppercase::No ? "{:016x}" : "{:016X}"),
                std::make_format_args(
                    binary[0], binary[1], binary[2], binary[3], binary[4], binary[5], binary[6], binary[7]));
        }

        uint64_t operator[](int index) const noexcept { return binary[static_cast<uint64_t>(index)]; }

        bool operator==(const sha512digest &that) const noexcept { return binary == that.binary; }

        std::array<uint64_t, 8> binary{};
    };

    static_assert(sizeof(sha512digest) == 64);


    export class sha512 final
    {
    public:
        sha512() { reset(); }
        void reset() noexcept
        {
            m_state      = {0x6a09e667f3bcc908,
                            0xbb67ae8584caa73b,
                            0x3c6ef372fe94f82b,
                            0xa54ff53a5f1d36f1,
                            0x510e527fade682d1,
                            0x9b05688c2b3e6c1f,
                            0x1f83d9abfb41bd6b,
                            0x5be0cd19137e2179};
            m_block      = {};
            m_bitlen     = 0ULL;
            m_blockindex = 0ULL;
        }
        void update(std::span<const char> const data) noexcept { generic_update<const char>(data); }
        void update(std::span<uint8_t> const data) noexcept { generic_update<uint8_t>(data); }

        sha512digest finalize() noexcept
        {
            sha512digest ret;

            pad();
            ret.binary = m_state;
            reset();
            return ret;
        }

    private:
        template <typename T> void generic_update(const std::span<T> data) noexcept
        {
            if (data.empty())
                return;

            for (const auto &i : data)
            {
                m_block[m_blockindex++] = static_cast<uint8_t>(i);
                if (m_blockindex == SHA512_BLOCK_SIZE)
                {
                    transform();

                    m_bitlen += SHA512_CHUNK_SIZE_IN_BITS;
                    m_blockindex = 0;
                }
            }
        }
        static uint64_t choose(uint64_t e, uint64_t f, uint64_t g) noexcept { return (e & f) ^ ((~e) & g); }
        static uint64_t majority(uint64_t a, uint64_t b, uint64_t c) noexcept { return (a & b) ^ (a & c) ^ (b & c); }
        static uint64_t sig0(uint64_t x) noexcept { return (std::rotr(x, 1) ^ std::rotr(x, 8) ^ (x >> 7)); }
        static uint64_t sig1(uint64_t x) noexcept { return (std::rotr(x, 19) ^ std::rotr(x, 61) ^ (x >> 6)); }


        void transform()   // Process the message in successive 512-bit chunks
        {
            uint64_t                maj{}, S0{}, ch{}, S1{}, temp1{}, temp2{}, w[SHA512_ROUNDS]{0};
            std::array<uint64_t, 8> state;

            // copy chunk into first 16 words w[0..15] of the message schedule array
            for (uint64_t i = 0, j = 0; i < 16; i++, j += 8)
                w[i] = load_bigendian<uint64_t>(&m_block[j]);


            // Initialize working variables to current hash value
            state = m_state;

            // Compression function main loop
            for (uint8_t i = 0; i < SHA512_ROUNDS; i++)
            {
                if (i >= 16)
                {
                    uint64_t s0 = sha512::sig0(w[i - 15]);
                    uint64_t s1 = sha512::sig1(w[i - 2]);
                    w[i]        = w[i - 16] + s0 + w[i - 7] + s1;
                }
                S1    = std::rotr(state[4], 14) ^ std::rotr(state[4], 18) ^ std::rotr(state[4], 41);
                ch    = choose(state[4], state[5], state[6]);
                temp1 = state[7] + S1 + ch + K[i] + w[i];
                S0    = std::rotr(state[0], 28) ^ std::rotr(state[0], 34) ^ std::rotr(state[0], 39);
                maj   = sha512::majority(state[0], state[1], state[2]);
                temp2 = S0 + maj;

                state[7] = state[6];           // h = g
                state[6] = state[5];           // g = f
                state[5] = state[4];           // f = e
                state[4] = state[3] + temp1;   // d + temp1
                state[3] = state[2];           // d = c
                state[2] = state[1];           // c = b
                state[1] = state[0];           // b = a
                state[0] = temp1 + temp2;      // a = temp1 + temp2
            }

            // Add the compressed chunk to the current hash value
            for (uint32_t i = 0; i < 8; i++)
                m_state[i] += state[i];
        }

        void pad()
        {
            uint64_t i = m_blockindex;

            if (m_blockindex < (SHA512_BLOCK_SIZE - 8))
            {
                m_block[i++] = 0x80;
                while (i < (SHA512_BLOCK_SIZE - 8))
                    m_block[i++] = 0x00;
            }
            else
            {
                m_block[i++] = 0x80;
                while (i < (SHA512_BLOCK_SIZE - 8))
                    m_block[i++] = 0x00;

                transform();
                std::fill_n(&m_block[0], SHA512_BLOCK_SIZE - 8, static_cast<uint8_t>(0));
            }

            m_bitlen += m_blockindex * 8;

            m_block[SHA512_BLOCK_SIZE - 1] = static_cast<uint8_t>((uint64_t)m_bitlen >> 0);
            m_block[SHA512_BLOCK_SIZE - 2] = static_cast<uint8_t>((uint64_t)m_bitlen >> 8);
            m_block[SHA512_BLOCK_SIZE - 3] = static_cast<uint8_t>((uint64_t)m_bitlen >> 16);
            m_block[SHA512_BLOCK_SIZE - 4] = static_cast<uint8_t>((uint64_t)m_bitlen >> 24);
            m_block[SHA512_BLOCK_SIZE - 5] = static_cast<uint8_t>((uint64_t)m_bitlen >> 32);
            m_block[SHA512_BLOCK_SIZE - 6] = static_cast<uint8_t>((uint64_t)m_bitlen >> 40);
            m_block[SHA512_BLOCK_SIZE - 7] = static_cast<uint8_t>((uint64_t)m_bitlen >> 48);
            m_block[SHA512_BLOCK_SIZE - 8] = static_cast<uint8_t>((uint64_t)m_bitlen >> 56);

            transform();
        }

        std::array<uint8_t, SHA512_BLOCK_SIZE> m_block;
        std::array<uint64_t, 8>                m_state;
        uint64_t                               m_bitlen;
        uint32_t                               m_blockindex;


        static constexpr std::array<uint64_t, SHA512_ROUNDS> K
            = {0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
               0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
               0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
               0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
               0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
               0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
               0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
               0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
               0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
               0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
               0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
               0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
               0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c,
               0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6,
               0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
               0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};
    };

    static_assert(sizeof(sha512) == 208);
    constexpr auto s = sizeof(sha512);


}   // namespace hash
