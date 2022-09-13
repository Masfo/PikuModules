module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <format>
#include <string>
#include <string_view>
#include <type_traits>

export module piku.win32;

using namespace std::string_literals;

std::string GetLocalRegistryValue(std::string_view key, std::string_view value) noexcept;

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
        return reinterpret_cast<T>((FARPROC *)func);
    }


    std::string OSVersion() noexcept
    {
        const auto key = R"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)"s;

        auto ProductName    = GetLocalRegistryValue(key, "ProductName");
        auto ReleaseId      = GetLocalRegistryValue(key, "ReleaseId");
        auto DisplayVersion = GetLocalRegistryValue(key, "DisplayVersion");
        auto CurrentBuild   = GetLocalRegistryValue(key, "CurrentBuild");
        auto UBR            = GetLocalRegistryValue(key, "UBR");

        if (ProductName.empty() && ReleaseId.empty() && CurrentBuild.empty() && UBR.empty())
            return "Windows";

        return std::format("Microsoft Windows Version {} (OS Build {}.{}) ",
                           DisplayVersion.empty() ? ReleaseId : DisplayVersion,
                           CurrentBuild,
                           UBR);
    }

}   // namespace piku


std::string GetLocalRegistryValue(std::string_view key, std::string_view value) noexcept
{
    std::string ret;

    HKEY hKey{};
    auto dwError = RegOpenKeyExA(HKEY_LOCAL_MACHINE, key.data(), 0u, (REGSAM)KEY_READ | KEY_WOW64_64KEY, &hKey);
    if (dwError != ERROR_SUCCESS)
        return "";

    ULONG cb   = 0;
    ULONG Type = 0;

    dwError = RegQueryValueExA(hKey, value.data(), nullptr, &Type, nullptr, &cb);
    if (dwError != ERROR_SUCCESS)
        return "";

    if (Type == REG_DWORD)
    {
        unsigned long regvalue = 0;

        dwError = RegQueryValueExA(hKey, value.data(), nullptr, &Type, (PBYTE)&regvalue, &cb);
        if (dwError != ERROR_SUCCESS)
            return "";

        return std::to_string(regvalue);
    }

    if (Type == REG_SZ)
    {
        ret.resize(cb);

        dwError = RegQueryValueExA(hKey, value.data(), nullptr, &Type, (PBYTE)ret.data(), &cb);
        if (dwError != ERROR_SUCCESS)
            return "";

        ret.resize(ret.size() - 1);

        return ret;
    }

    return "";
}