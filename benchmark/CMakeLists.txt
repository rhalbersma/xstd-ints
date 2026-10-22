#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include(FetchContent)

# nanobench v4.3.11, pinned by commit; a nanobench that find_package already reaches, such as vcpkg's, is taken instead.
FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG a5a50c2b33eea2ff1fcb355cacdface43eb42b25
    FIND_PACKAGE_ARGS CONFIG
)
# zp7, pinned by commit: a rejected alternative timed as its author wrote it, compiled into the benchmark alone.
FetchContent_Declare(
    zp7
    GIT_REPOSITORY https://github.com/zwegner/zp7.git
    GIT_TAG a1ed4e5ace07f7d69cb50af1cbd37df4fa3d87af
)
FetchContent_MakeAvailable(nanobench zp7)

# A results table names the commit it measured and the flags it was built with, so both are read here.
find_package(Git QUIET)
set(bench_commit "unknown")
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --always --dirty --abbrev=12
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE bench_commit
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" bench_build_type)
string(STRIP "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${bench_build_type}}" bench_flags)

add_executable(bench_bit_permutation bit_permutation.cpp)

target_include_directories(
    bench_bit_permutation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${zp7_SOURCE_DIR}
)

target_link_libraries(
    bench_bit_permutation PRIVATE
    xstd::ints
    nanobench::nanobench
)

target_compile_definitions(
    bench_bit_permutation PRIVATE
    XSTD_INTS_BENCH_COMMIT="${bench_commit}"
    XSTD_INTS_BENCH_FLAGS="${bench_flags}"
)

target_compile_options(
    bench_bit_permutation PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
)
