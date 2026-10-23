#          Copyright Rein Halbersma 2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The dialect the suites' markers are read against, and the one the library's own tests build at.
set(CMAKE_CXX_STANDARD 23)

# cpp-ci at the commit the clang ladder pins, so libc++'s tag is the one its stable rung builds with.
file(STRINGS "${PROJECT_SOURCE_DIR}/.github/workflows/clang.yml" cpp_ci_uses REGEX "uses: rhalbersma/cpp-ci/[^@]+@[0-9a-f]+" LIMIT_COUNT 1)
if(NOT cpp_ci_uses MATCHES "@([0-9a-f]+)")
    message(FATAL_ERROR "no cpp-ci pin in .github/workflows/clang.yml")
endif()

include(FetchContent)
FetchContent_Declare(
    cpp-ci
    GIT_REPOSITORY https://github.com/rhalbersma/cpp-ci.git
    GIT_TAG ${CMAKE_MATCH_1}
)
FetchContent_MakeAvailable(cpp-ci)
include(${cpp-ci_SOURCE_DIR}/cmake/StdTestSuite.cmake)

# The 128-bit class types the tests also run over, beside the bit-precise widths include/xstd_types.hpp names.
include(${PROJECT_SOURCE_DIR}/test/cmake/int128.cmake)
if(NOT ${project_id}_test_boost_int128 OR NOT ${project_id}_test_absl_int128)
    message(FATAL_ERROR "The standard library test suites run over Boost.Int128's and Abseil's 128-bit types; find or fetch both")
endif()

add_library(${project_id}_std_test_suite_types INTERFACE)
target_include_directories(${project_id}_std_test_suite_types INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${project_id}_std_test_suite_types INTERFACE xstd::ints ${${project_id}_test_boost_int128} ${${project_id}_test_absl_int128})

# Each suite against the standard library it is written for: libc++'s tests reach libc++'s own extensions, and
# libstdc++'s support headers include its internal ones.
include(CheckCXXSymbolExists)
check_cxx_symbol_exists(_LIBCPP_VERSION version ${project_id}_std_test_suite_libcpp)
if(${project_id}_std_test_suite_libcpp)
    std_test_suite(libcxx MAPPING libcxx.toml LINK_LIBRARIES ${project_id}_std_test_suite_types)
else()
    std_test_suite(libstdcxx MAPPING libstdcxx.toml LINK_LIBRARIES ${project_id}_std_test_suite_types)
endif()
