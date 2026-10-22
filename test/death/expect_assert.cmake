#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Run as a death test's launcher, cmake -P expect_assert.cmake -- <command>, the command being the emulator, where
# there is one, and the test executable. CTest reports a process killed by a signal as an exception, which neither
# WILL_FAIL nor PASS_REGULAR_EXPRESSION reaches, so this turns the abort into a result: the test passes only where
# the command fails an assert in this library, and fails where it returns, or ends any other way, such as a
# sanitizer's report.

cmake_minimum_required(VERSION 3.30)

# Everything after the "--" that CTest's launcher line ends in.
set(command)
set(past_separator FALSE)
math(EXPR last_argument "${CMAKE_ARGC} - 1")
foreach(i RANGE ${last_argument})
    if(past_separator)
        list(APPEND command "${CMAKE_ARGV${i}}")
    elseif(CMAKE_ARGV${i} STREQUAL "--")
        set(past_separator TRUE)
    endif()
endforeach()

execute_process(
    COMMAND ${command}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)

# glibc puts the file before "Assertion `...' failed."; the BSD, musl and Emscripten C libraries put it after.
set(library_assert "([^\n]*/xstd/ints/[^\n]*Assertion[^\n]*failed)|(Assertion failed[^\n]*/xstd/ints/)")

set(in_library FALSE)
if(output MATCHES "${library_assert}")
    set(in_library TRUE)
elseif(output MATCHES "Assertion failed: [^\n]*, file ([^,/\n]+), line")
    # Apple's C library names the file without its directory, so the name is looked for among this library's headers.
    file(GLOB_RECURSE library_header "${CMAKE_CURRENT_LIST_DIR}/../../include/xstd/ints/${CMAKE_MATCH_1}")
    if(library_header)
        set(in_library TRUE)
    endif()
endif()

if(result EQUAL 0)
    message(FATAL_ERROR "${command} returned instead of failing an assert:\n${output}")
elseif(NOT in_library)
    message(FATAL_ERROR "${command} ended in \"${result}\", not in a failed assert in xstd/ints:\n${output}")
endif()
message(STATUS "${command} failed an assert, as expected:\n${output}")
