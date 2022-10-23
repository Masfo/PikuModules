module;
#include <array>
#include <span>


export module piku.crypto;
import piku.types;

using namespace piku;

namespace salsa20
{
    constexpr u32 KEY_LENGTH   = 32;
    constexpr u32 NONCE_LENGTH = 8;


    export struct key
    {
        std::array<u8, KEY_LENGTH> binary;
    };

    export struct nonce
    {
        std::array<u8, NONCE_LENGTH> binary;
    };


    export bool encrypt(key const &/*k*/, nonce const &/*n*/, std::span<u8> const /*buffer*/) noexcept { return true; }

    export bool decrypt(key const &k, nonce const &n, std::span<u8> const buffer) noexcept
    {
        return encrypt(k, n, buffer);
    }


}   // namespace salsa20