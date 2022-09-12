module;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <format>
#include <iostream>
#include <string_view>
#include <source_location>
export module piku.debug;

using namespace std::string_view_literals;
// println
// print
// trace
// assert
void output_message(const std::string_view message) { OutputDebugStringA(message.data()); }

struct FormatLocation
{
    std::string_view     fmt;
    std::source_location loc;
    FormatLocation(const char *s, const std::source_location &l = std::source_location::current()) : fmt(s), loc(l) {}
    FormatLocation(const std::string_view s, const std::source_location &l = std::source_location::current())
        : fmt(s), loc(l)
    {
    }
};

export namespace piku
{

    template <typename... Args> void trace(FormatLocation fmt, Args &&...args) noexcept
    {

        output_message(std::format("{}({}): {}\n"sv,
                                   fmt.loc.file_name(),
                                   fmt.loc.line(),
                                   std::vformat(fmt.fmt, std::make_format_args(args...))));
    }

    void trace(FormatLocation fmt) noexcept
    {
        output_message(std::format("{}({}): {}\n"sv, fmt.loc.file_name(), fmt.loc.line(), fmt.fmt));
    }

    void trace() noexcept { output_message("\n"); }

}   // namespace piku
