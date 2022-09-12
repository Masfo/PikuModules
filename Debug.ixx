module;
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

void output_message(const std::string_view message) { std::cout << message << std::endl; }

export template <typename... Args>
void trace2(const std::string_view fmt,
            Args &&...args,
            const std::source_location &loc = std::source_location::current()) noexcept
{
    if (fmt != "\n"sv)
    {
        output_message(std::format(
            "{}({:6}): {}\n"sv, loc.file_name(), loc.line(), std::vformat(fmt, std::make_format_args(args...))));
    }
    else
    {
        output_message(fmt);
    }
}


#if 0
// Trace
template <typename T, typename... Ts> struct trace
{
    static_assert(std::is_convertible_v<T, std::string_view>, "ASSERT: First argument is not a format string");
    explicit trace(T &&fmts, Ts &&...ts, const std::source_location &loc = std::source_location::current()) noexcept
    {
        detail::output_trace(compose_debug_line(loc, fmts, ts...));
    }
};
template <typename... Ts> trace(Ts &&...) -> trace<Ts...>;
#endif