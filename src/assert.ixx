module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <format>
#include <iostream>
#include <string_view>
#include <source_location>

export module piku.assert;

import piku.debug;
using namespace piku;

#ifdef _DEBUG

export void assert_msg(bool                        expr,
                       std::string_view            message,
                       const std::source_location &loc = std::source_location::current()) noexcept
{
    if (expr)
        return;

    println("\nAssert *****\n\n {}({}): ", loc.file_name(), loc.line());

    println("{}", message);

    println("\n\nAssert *****\n\n");

    if (IsDebuggerPresent())
    {
        DebugBreak();
        FatalExit(0);
    }
}


export void assert(bool expr, const std::source_location &loc = std::source_location::current()) noexcept
{
    assert_msg(expr, "", loc);
}


#else
void assert_msg(bool, std::string_view) noexcept {}
void assert(bool) noexcept {}

#endif