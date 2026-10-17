#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

add_executable(perft perft.cpp)
target_include_directories(perft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(perft PRIVATE xstd::ints)
target_compile_options(
    perft PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
)

# The quick suite, a few seconds of nodes; `perft full` runs each position to depths that take minutes.
add_test(NAME examples.perft COMMAND perft)
