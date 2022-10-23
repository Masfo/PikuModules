module;
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX
#include <windows.h>

#include <filesystem>
#include <string_view>

export module piku.file;

import piku.types;

namespace piku
{
    namespace fs = std::filesystem;
    class Fileview final
    {
    public:
        explicit Fileview(fs::path const filename) {

        }
    private:
    };


}   // namespace piku