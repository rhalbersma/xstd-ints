#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Runs an example and compares what it prints with the output its reference page shows, line endings aside, so the
# check reads the same on Windows as elsewhere:
#
#   cmake -Dexample=<executable> -Dexpected=<file> [-Demulator=<program>] -P check_output.cmake
#
# The emulator is the example's CROSSCOMPILING_EMULATOR, node under Emscripten, which add_test would otherwise prepend.

foreach(argument example expected)
    if(NOT DEFINED ${argument})
        message(FATAL_ERROR "check_output.cmake needs -D${argument}=...")
    endif()
endforeach()

execute_process(
    COMMAND ${emulator} ${example}
    OUTPUT_VARIABLE printed
    ERROR_VARIABLE diagnostics
    RESULT_VARIABLE status
)
if(NOT status EQUAL 0)
    message(FATAL_ERROR "${example} exited with ${status}\n${diagnostics}")
endif()

file(READ ${expected} shown)
string(REPLACE "\r\n" "\n" printed "${printed}")
string(REPLACE "\r\n" "\n" shown "${shown}")
if(NOT printed STREQUAL shown)
    message(FATAL_ERROR "${example} printed\n${printed}\nwhere ${expected} shows\n${shown}")
endif()
