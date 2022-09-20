module;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <format>
#include <iostream>
#include <string_view>
#include <source_location>
#include <version>
#include "prototyping_markers.h"
#ifdef __cpp_lib_stacktrace
#    include <stacktrace>
#    define HAS_STACKTRACE
#    pragma FIX("We have stacktrace, remove guards")

#endif


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

    println("\nAssert *****\n\n {}({}): {}", loc.file_name(), loc.line(), message);

#    ifdef HAS_STACKTRACE
    auto traces = std::stacktrace::current();

    for (const auto &trace : traces)
    {
        if (trace.source_file().contains(__FILE__))
            continue;

        println("{}({}): {}", trace.source_file(), trace.source_line(), trace.description());
    }
#    endif
    println("\nAssert *****\n\n");

    if (IsDebuggerPresent())
    {
        DebugBreak();
        FatalExit(0);
    }
}


export void assert(bool expr = false, const std::source_location &loc = std::source_location::current()) noexcept
{
    assert_msg(expr, "", loc);
}


#else
export void assert_msg(bool, std::string_view) noexcept {}
export void assert(bool) noexcept {}

#endif