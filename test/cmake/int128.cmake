#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Sets ${project_id}_test_boost_int128 and ${project_id}_test_absl_int128 to each library's target, or to nothing where
# it is missing. Included from more than one directory, so each target is created once and found after that.

# The one 128-bit type the suite tests against that xstd knows nothing about,
# and not a hard dependency. A pre-release library in no Boost release yet,
# so CI and a local build alike take the fetch below.
option(${project_option_prefix}_TEST_FETCH_BOOST_INT128 "Fetch Boost.Int128 when no installed copy is found" ON)

find_package(boost_int128 CONFIG QUIET)
if(NOT TARGET Boost::int128 AND ${project_option_prefix}_TEST_FETCH_BOOST_INT128)
    include(FetchContent)
    FetchContent_Declare(
        boost_int128
        GIT_REPOSITORY https://github.com/cppalliance/int128.git
        # A development commit rather than v3.1.0, the types having lost their
        # _t suffix after it was cut. Pinned rather than tracking a branch.
        GIT_TAG f437657470186b79f1f314bc2c76a112bcc2529c
        # Its CMakeLists.txt builds its own test suite under BUILD_TESTING;
        # naming a subdirectory without one keeps the download and drops that.
        SOURCE_SUBDIR does-not-exist
    )
    FetchContent_MakeAvailable(boost_int128)

    # SYSTEM because these headers are not written to this project's warning
    # level; an installed copy arrives as an imported target, already system.
    # GLOBAL because the unit tests and the standard library test suites both include this file.
    add_library(Boost::int128 INTERFACE IMPORTED GLOBAL)
    target_include_directories(Boost::int128 SYSTEM INTERFACE ${boost_int128_SOURCE_DIR}/include)
endif()

if(TARGET Boost::int128)
    set(${project_id}_test_boost_int128 Boost::int128)
else()
    set(${project_id}_test_boost_int128)
endif()

# The other half of the same extension point, and not a hard dependency
# either: absl::int128 declares no noexcept anywhere, which is the case the
# integer functions' conditional exception specification exists for.
option(${project_option_prefix}_TEST_FETCH_ABSL_INT128 "Fetch Abseil when no installed copy is found" ON)

find_package(absl CONFIG QUIET)
if(NOT TARGET absl::int128 AND ${project_option_prefix}_TEST_FETCH_ABSL_INT128)
    include(FetchContent)
    FetchContent_Declare(
        absl_int128
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        # A release tag, the spelling of these two types being settled.
        GIT_TAG 20260526.0
        # Abseil's CMakeLists.txt configures the whole library and its tests;
        # naming a subdirectory without one keeps the download and drops that,
        # leaving the one translation unit these two types need.
        SOURCE_SUBDIR does-not-exist
    )
    FetchContent_MakeAvailable(absl_int128)

    # int128.cc is where operator/ and operator% live on a platform with no
    # 128-bit intrinsic to lower them to; the rest of the type is header-only.
    add_library(${project_id}_test_absl_int128_lib STATIC ${absl_int128_SOURCE_DIR}/absl/numeric/int128.cc)

    # Built to the same standard as the tests, not to the compiler's default:
    # absl::strong_ordering is std::strong_ordering only where the library
    # sees C++20's <compare>, and the two spellings must not meet at a link.
    target_compile_features(${project_id}_test_absl_int128_lib PUBLIC cxx_std_23)

    # SYSTEM for the reason Boost::int128's includes are.
    target_include_directories(${project_id}_test_absl_int128_lib SYSTEM PUBLIC ${absl_int128_SOURCE_DIR})

    add_library(absl::int128 ALIAS ${project_id}_test_absl_int128_lib)
endif()

if(TARGET absl::int128)
    set(${project_id}_test_absl_int128 absl::int128)
else()
    set(${project_id}_test_absl_int128)
endif()
