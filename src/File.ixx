module;

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <filesystem>
#include <string_view>
#include <span>

export module piku.file;

import piku.assert;

namespace piku
{
    export enum class FileAccess {
        Read,
        ReadWrite,
    };
    namespace fs = std::filesystem;
    export class Fileview final
    {
    public:
        Fileview()                 = default;

        Fileview(const Fileview &) = delete;

        Fileview(Fileview &&o) noexcept : m_filehandle(o.m_filehandle), m_mappinghandle(o.m_mappinghandle)
        {
            o.m_filehandle = o.m_mappinghandle = 0;
        }

        Fileview &operator=(const Fileview &) = delete;
        Fileview &operator=(Fileview &&o) noexcept
        {
            if (this != &o)
            {
                unmap();
                m_filehandle    = o.m_filehandle;
                m_mappinghandle = o.m_mappinghandle;
                o.m_filehandle = o.m_mappinghandle = 0;
            }
            return *this;
        }
        explicit Fileview(fs::path const filename) noexcept { open(filename); }

        bool open(fs::path const filename, FileAccess rw= FileAccess::Read) noexcept
        {
            DWORD access    = GENERIC_READ;
            DWORD page_protect = PAGE_READONLY;
            
            if (rw == FileAccess::ReadWrite)
            {
                access |= GENERIC_WRITE;
                page_protect = PAGE_READWRITE;
                m_readonly   = false;
            }

            m_filehandle = CreateFile(
                filename.wstring().c_str(), access, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            m_mappinghandle = CreateFileMapping(m_filehandle, nullptr, page_protect, 0, 0, nullptr);

            assert_msg(m_mappinghandle != 0, "Could not open file mapping");

            return is_open();
        }

        bool is_open() const noexcept { return m_mappinghandle != 0; }

        operator bool() const noexcept { return is_open(); }

        size_t size() const noexcept
        {
            LARGE_INTEGER fs;
            if (GetFileSizeEx(m_filehandle, &fs) != 0)
                return fs.QuadPart;
            return 0;
        }

        auto stringview() const noexcept
        {
            if (auto *addr = static_cast<char *>(map()); addr)
                return std::string_view{addr, size()};

            return std::string_view{};
        }
        auto span() const noexcept
        {
            if (auto *addr = static_cast<std::byte *>(map()); addr)
                return std::span<std::byte>{addr, size()};

            return std::span<std::byte>{};
        }

        ~Fileview()
        {
            unmap();
            m_readonly = true;
        }

        bool operator==(const Fileview &other) const = default;

    private:
        void *map() const noexcept
        {
            assert_msg(m_mappinghandle != 0, "File mapping is not open");

            if (auto *addr = MapViewOfFile(m_mappinghandle, FILE_MAP_READ, 0, 0, 0); addr != nullptr)
                return addr;

            return nullptr;
        }
        void unmap() noexcept
        {
            CloseHandle(m_mappinghandle);
            CloseHandle(m_filehandle);
            m_filehandle = m_mappinghandle = 0;
        }
        HANDLE m_filehandle{0};
        HANDLE m_mappinghandle{0};
        bool   m_readonly{true};
    };


}   // namespace piku