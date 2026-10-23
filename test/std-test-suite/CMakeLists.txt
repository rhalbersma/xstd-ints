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

std_test_suite(libcxx MAPPING libcxx.toml LINK_LIBRARIES xstd::ints)
