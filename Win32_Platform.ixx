module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string_view>
#include <type_traits>

export module piku_win32;


export namespace piku
{
    template <typename T>
    requires std::is_pointer_v<T> T LoadDynamic(std::string_view dll, std::string_view function)
    {
        auto lib = LoadLibraryA(dll.data());
        if (!lib)
            return nullptr;

        auto func = GetProcAddress(lib, function.data());
        if (!func)
            return nullptr;
        return reinterpret_cast<T>(func);
    }
}   // namespace piku