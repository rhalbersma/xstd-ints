#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include(FetchContent)

# sux and SDSL, each pinned by commit, are read as headers only: SOURCE_SUBDIR names no directory, so neither build runs.
FetchContent_Declare(
    sux
    GIT_REPOSITORY https://github.com/vigna/sux.git
    GIT_TAG 568903f1b7957ef03620ebad65d6ef68e031fef9
    SOURCE_SUBDIR headers-only
)
FetchContent_Declare(
    sdsl
    GIT_REPOSITORY https://github.com/xxsds/sdsl-lite.git
    GIT_TAG 5a7e423b200f2f81ba27ceb47986fe9fa30a091b
    GIT_SUBMODULES ""
    SOURCE_SUBDIR headers-only
)
FetchContent_MakeAvailable(sux sdsl)

add_executable(bench_select select_benchmark.cpp)

target_include_directories(bench_select PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(
    bench_select SYSTEM PRIVATE
    ${sux_SOURCE_DIR}
    ${sdsl_SOURCE_DIR}/include
)

target_link_libraries(bench_select PRIVATE xstd::ints)

# GCC reports sux's Rank9Sel constructor as reading uninitialized locals once inlined here, where SYSTEM cannot reach it.
target_compile_options(
    bench_select PRIVATE
    -Wall -Wextra
    $<$<CXX_COMPILER_ID:GNU>:-Wno-maybe-uninitialized>
)
