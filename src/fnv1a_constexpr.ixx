module;
#include <cstddef>
#include <string_view>

export module hash.fnv;
import piku.types;

using namespace piku;

namespace hash
{

    constexpr u32 val_32_const   = 0x811c9dc5;
    constexpr u32 prime_32_const = 0x1000193;
    constexpr u64 val_64_const   = 0xcbf29ce484222325;
    constexpr u64 prime_64_const = 0x100000001b3;


    export constexpr u32 fnv1a_32(char const *s, size_t count) noexcept
    {
        return count ? (fnv1a_32(s, count - 1) ^ s[count - 1]) * prime_32_const : val_32_const;
    }

    export constexpr u64 fnv1a_64(char const *s, size_t count) noexcept
    {
        return count ? (fnv1a_64(s, count - 1) ^ s[count - 1]) * prime_64_const : val_64_const;
    }


    export constexpr u32 fnv1a_32(std::string_view str) noexcept { return fnv1a_32(str.data(), str.length()); }
    export constexpr u64 fnv1a_64(std::string_view str) noexcept { return fnv1a_64(str.data(), str.length()); }


}   // namespace hash


namespace hash::literals
{
    export constexpr u32 operator"" _hash32(char const *s, size_t count) { return fnv1a_32(s, count); }
    export constexpr u64 operator"" _hash64(char const *s, size_t count) { return fnv1a_64(s, count); }

    static_assert("hello world"_hash32 == 0xd58b3fa7);
    static_assert("hello world"_hash64 == 0x779a65e7023cd2e7);

}   // namespace hash::literals