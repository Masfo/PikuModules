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
        Fileview() = default;

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

        bool open(fs::path const filename, FileAccess rw = FileAccess::Read) noexcept
        {
            DWORD access       = GENERIC_READ;
            DWORD page_protect = PAGE_READONLY;

            if (rw == FileAccess::ReadWrite)
            {
                access |= GENERIC_WRITE;
                page_protect = PAGE_READWRITE;
                m_readonly   = false;
            }

            m_filehandle    = CreateFile(filename.wstring().c_str(),
                                      access,
                                      FILE_SHARE_READ,
                                      nullptr,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL,
                                      nullptr);
            m_mappinghandle = CreateFileMapping(m_filehandle, nullptr, page_protect, 0, 0, nullptr);

            assert_msg(m_mappinghandle != 0, "Could not open file mapping");

            map();

            return is_open();
        }

        bool is_open() const noexcept { return m_mappinghandle != 0; }

        operator bool() const noexcept { return is_open(); }

        void close() noexcept
        {
            unmap();
            m_readonly = true;
        }

        size_t size() const noexcept
        {
            LARGE_INTEGER fs;
            if (GetFileSizeEx(m_filehandle, &fs) != 0)
                return fs.QuadPart;
            return 0;
        }

        auto stringview() noexcept
        {
            if (auto *addr = static_cast<char *>(map()); addr)
                return std::string_view{addr, size()};

            return std::string_view{};
        }
        auto span() noexcept
        {
            if (auto *addr = static_cast<char *>(map()); addr)
                return std::span<char>{addr, size()};

            return std::span<char>{};
        }

       auto &operator[](size_t index) noexcept
        {
            auto ptr = static_cast<char*>(map());
            return ptr[index];
        }

        ~Fileview() { close();
        }

        bool operator==(const Fileview &other) const = default;

    private:
        void *map()  noexcept
        {
            assert_msg(m_mappinghandle != 0, "File mapping is not open");

            if (m_addr != nullptr)
                return m_addr;

            DWORD access = FILE_MAP_READ;
            if (m_readonly == false)
                access |= FILE_MAP_WRITE;

            m_addr = MapViewOfFile(m_mappinghandle, access, 0, 0, 0);
            return m_addr;
        }
        void unmap() noexcept
        {
            FlushViewOfFile(m_addr, 0);
            UnmapViewOfFile(m_addr);
            CloseHandle(m_mappinghandle);
            CloseHandle(m_filehandle);
            m_addr = m_filehandle = m_mappinghandle = 0;
        }
        HANDLE m_filehandle{0};
        HANDLE m_mappinghandle{0};
        void  *m_addr { nullptr };
        bool   m_readonly{true};
    };


}   // namespace piku