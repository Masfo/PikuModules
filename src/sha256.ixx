module;
#include <array>
#include <bit>
#include <cstdint>
#include <format>
#include <string>
#include <span>

export module hash.sha256;


constexpr uint32_t BLOCK_SIZE        = 64;
constexpr uint32_t BINARY_BLOCK_SIZE = BLOCK_SIZE / 8;

export enum Uppercase : uint8_t {
    No,
    Yes,
};

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

    std::array<uint32_t, BINARY_BLOCK_SIZE> binary{};
};

static_assert(sizeof(sha256digest) == 32);

export class sha256 final
{
public:
    sha256() { reset(); }
    void reset() noexcept
    {
        m_state    = {0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19};
        m_data     = {};
        m_bitlen   = 0ULL;
        m_blocklen = 0ULL;
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
    template <typename T> void generic_update(std::span<T> data) noexcept
    {
        if (data.empty())
            return;

        for (const auto &i : data)
        {
            m_data[m_blocklen++] = static_cast<uint8_t>(i);
            if (m_blocklen == BLOCK_SIZE)
            {
                transform();

                m_bitlen += 512;
                m_blocklen = 0;
            }
        }
    }
    static uint32_t choose(uint32_t e, uint32_t f, uint32_t g) noexcept { return (e & f) ^ (~e & g); }
    static uint32_t majority(uint32_t a, uint32_t b, uint32_t c) noexcept { return (a & (b | c)) | (b & c); }
    static uint32_t sig0(uint32_t x) noexcept { return std::rotr(x, 7) ^ std::rotr(x, 18) ^ (x >> 3); }
    static uint32_t sig1(uint32_t x) noexcept { return std::rotr(x, 17) ^ std::rotr(x, 19) ^ (x >> 10); }

    void transform()
    {
        uint32_t maj{}, xorA{}, ch{}, xorE{}, sum{}, newA{}, newE{}, m[64]{0};
        uint32_t state[8]{0};

        for (uint32_t i = 0, j = 0; i < 16; i++, j += 4)
            m[i] = static_cast<uint32_t>((m_data[j] << 24u) | (m_data[j + 1u] << 16u) | (m_data[j + 2u] << 8u)
                                         | (m_data[j + 3u]));

        for (uint8_t k = 16; k < 64; k++)
            m[k] = sha256::sig1(m[k - 2]) + m[k - 7] + sha256::sig0(m[k - 15]) + m[k - 16];

        for (uint8_t i = 0; i < 8; i++)
            state[i] = m_state[i];

        for (uint8_t i = 0; i < 64; i++)
        {
            maj  = sha256::majority(state[0], state[1], state[2]);
            xorA = std::rotr(state[0], 2) ^ std::rotr(state[0], 13) ^ std::rotr(state[0], 22);

            ch = choose(state[4], state[5], state[6]);

            xorE = std::rotr(state[4], 6) ^ std::rotr(state[4], 11) ^ std::rotr(state[4], 25);

            sum  = m[i] + K[i] + state[7] + ch + xorE;
            newA = xorA + maj + sum;
            newE = state[3] + sum;

            state[7] = state[6];
            state[6] = state[5];
            state[5] = state[4];
            state[4] = newE;
            state[3] = state[2];
            state[2] = state[1];
            state[1] = state[0];
            state[0] = newA;
        }

        for (uint8_t i = 0; i < 8; i++)
            m_state[i] += state[i];
    }

    void pad()
    {

        uint64_t i   = m_blocklen;
        uint8_t  end = m_blocklen < (uint8_t)56 ? (uint8_t)56 : (uint8_t)64;

        m_data[i++] = 0x80;
        while (i < end)
            m_data[i++] = 0x00;

        if (m_blocklen >= 56)
        {
            transform();
            std::fill_n(m_data.begin(), 56, (uint8_t)0);
        }

        m_bitlen += m_blocklen * 8;
        m_data[63] = static_cast<uint8_t>(m_bitlen);
        m_data[62] = static_cast<uint8_t>(m_bitlen >> 8);
        m_data[61] = static_cast<uint8_t>(m_bitlen >> 16);
        m_data[60] = static_cast<uint8_t>(m_bitlen >> 24);
        m_data[59] = static_cast<uint8_t>(m_bitlen >> 32);
        m_data[58] = static_cast<uint8_t>(m_bitlen >> 40);
        m_data[57] = static_cast<uint8_t>(m_bitlen >> 48);
        m_data[56] = static_cast<uint8_t>(m_bitlen >> 56);

        transform();
    }

    std::array<uint8_t, BLOCK_SIZE>                   m_data;
    std::array<uint32_t, BINARY_BLOCK_SIZE>           m_state;
    uint64_t                                          m_bitlen;
    uint32_t                                          m_blocklen;
    static constexpr std::array<uint32_t, BLOCK_SIZE> K
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
