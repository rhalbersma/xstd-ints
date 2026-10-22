#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The project name as an identifier and as a cache-option prefix, so a rename carries the test tree with it.
string(MAKE_C_IDENTIFIER ${PROJECT_NAME} project_id)
# In that order: TOUPPER alone would leave XSTD-INTS, which is no name a cache option can have.
string(TOUPPER ${project_id} project_option_prefix)

add_library(${project_id}_test_options INTERFACE)
option(${project_option_prefix}_WARNINGS_AS_ERRORS "Treat warnings as errors in ${PROJECT_NAME} tests" ${PROJECT_IS_TOP_LEVEL})

# clang-cl reports CXX_COMPILER_ID Clang but only understands its MSVC-style
# driver flags, not GNU-style ones like -pedantic-errors.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(${project_id}_is_clang_cl ON)
else()
    set(${project_id}_is_clang_cl OFF)
endif()

set(cxx_compile_definitions
    BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE  # https://github.com/boostorg/config/issues/207
    BOOST_ALL_NO_LIB
    BOOST_TEST_MAIN
    $<$<CXX_COMPILER_ID:MSVC>:
        _SCL_SECURE_NO_WARNINGS
    >
)

set(cxx_compile_options_warnings
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /permissive-
    >
    $<$<CXX_COMPILER_ID:Clang>:
        -Weverything
        -Wno-unknown-warning-option         # forward compatibility for new Clang warnings
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        # The same category one standard on. Without it this and clang-tidy's
        # readability-redundant-typename deadlock over a single token.
        -Wno-c++20-compat
        -Wno-disabled-macro-expansion       # triggered by Boost.Test
        -Wno-global-constructors            # triggered by Boost.Test
        -Wno-used-but-marked-unused         # triggered by Boost.Test
        -Wno-c2y-extensions                 # triggered by Boost.Test's use of __COUNTER__
        -Wno-bit-int-extension              # _BitInt is C23; in C++ Clang alone has it
        -Wno-lifetime-safety-lifetimebound-violation  # can't verify chained operator<< returns ostr
        # Reads any reference to a container as a reference into it, so a
        # function that grows one twice - the div_result formatter's loop over
        # to_chars output - is a use after invalidation to it. Reached only
        # when an element type has no std::formatter of its own, which is why
        # absl::int128 is the first to draw it. Clang 23 and 24-SVN both.
        -Wno-lifetime-safety-invalidation
        -Wno-lifetime-safety-experimental-invalidation # the same warning, as Clang 24-SVN renames it under -Wlifetime-safety-experimental
        # xstd::to_chars takes a char* range because std::to_chars does, so
        # every pointer step inside that contract draws this.
        -Wno-unsafe-buffer-usage
    >
    $<$<CXX_COMPILER_ID:GNU>:
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wsign-compare
        -Wsign-conversion
        -Wsign-promo
        # A false positive inside <bits/unicode.h>, raised after inlining so
        # system-header suppression does not reach it. Scoped to GCC.
        -Wno-maybe-uninitialized
    >
    $<$<AND:$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>,$<NOT:$<BOOL:${${project_id}_is_clang_cl}>>>:
        -pedantic-errors
    >
)

target_compile_definitions(
    ${project_id}_test_options INTERFACE
    ${cxx_compile_definitions}
)
target_compile_options(
    ${project_id}_test_options INTERFACE
    ${cxx_compile_options_warnings}
    $<$<AND:$<BOOL:${${project_option_prefix}_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
    $<$<AND:$<BOOL:${${project_option_prefix}_WARNINGS_AS_ERRORS}>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-Werror>
    # Pre-empts MinGW's 32-byte aligned stores to a 16-byte aligned stack, which bite the
    # moment AVX is enabled. The assembler rewrites them unaligned, so 256-bit vectors
    # survive (rhalbersma/xstd-bits#54, GCC PR target/54412, binutils 2.38+).
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:-Wa$<COMMA>-muse-unaligned-vector-move>
)
target_include_directories(
    ${project_id}_test_options INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Each public header is compiled as its own translation unit (linked against
# the same warning flags as the unit tests), proving it is self-sufficient.
# Any target named after the header is one it needs to be self-sufficient against.
set(public_header_dir ${PROJECT_SOURCE_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency)

function(${project_id}_add_header_self_sufficiency_test h)
    string(REPLACE "/" "." header_test_id ${h})
    string(REGEX REPLACE "[.]hpp$" "" header_test_id ${header_test_id})
    set(header_test_source ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency/${header_test_id}.cpp)
    # A trailing return type because clang-tidy runs over these generated
    # sources too, and modernize-use-trailing-return-type flags "int main()".
    file(
        WRITE ${header_test_source}
        "#include <${h}> // IWYU pragma: keep\nauto main() -> int {}\n"
    )

    add_executable(header.${header_test_id} ${header_test_source})
    target_link_libraries(
        header.${header_test_id} PRIVATE
        xstd::ints
        ${ARGN}
        ${project_id}_test_options
    )
    add_test(NAME header.${header_test_id} COMMAND header.${header_test_id})
endfunction()

file(
    GLOB_RECURSE public_headers
    CONFIGURE_DEPENDS
    RELATIVE ${public_header_dir}
    ${public_header_dir}/xstd/*.hpp
)

# Every header but those under ext/, which cannot compile before their optional dependency resolves.
list(FILTER public_headers EXCLUDE REGEX "^xstd/ints/ext/")

# What the two-library leaf rule became once xstd/misc left for its own repository: this
# library still may not reach for it, and now an include would be an undeclared dependency
# on a package this one neither finds nor exports.
file(GLOB_RECURSE all_headers CONFIGURE_DEPENDS ${public_header_dir}/xstd/*.hpp)
foreach(h ${all_headers})
    file(STRINGS ${h} offending REGEX "#include[ \t]*<xstd/misc/")
    if(offending)
        message(FATAL_ERROR "${h} includes <xstd/misc/...>, which rhalbersma/xstd-misc ships and this package does not depend on.\n  ${offending}")
    endif()
endforeach()

foreach(h ${public_headers})
    cmake_language(CALL ${project_id}_add_header_self_sufficiency_test ${h})
endforeach()

# The test tree mirrors the header tree: every public header is answered by exactly
# one test source at the same path. A reviewer reading include/xstd/ints/cstdlib/abs.hpp
# knows where its tests are without looking. Enforced here so it cannot drift -- the
# glob above already lists every header, and ext/ is back in, its tests compiling
# against whichever of the two optional libraries is present.
file(
    GLOB_RECURSE all_public_headers
    CONFIGURE_DEPENDS
    RELATIVE ${public_header_dir}
    ${public_header_dir}/xstd/*.hpp
)

# A detail header has no contract to mirror: it is exercised through the public header that includes it.
list(FILTER all_public_headers EXCLUDE REGEX "/detail/")

set(unmirrored_headers)
foreach(h ${all_public_headers})
    # xstd/ints/cstdlib/abs.hpp is answered by test/src/ints/cstdlib/abs.cpp.
    string(REGEX REPLACE "^xstd/" "" mirrored ${h})
    string(REGEX REPLACE "[.]hpp$" ".cpp" mirrored ${mirrored})
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${mirrored})
        list(APPEND unmirrored_headers "    include/${h}\n      -> expected test/src/${mirrored}")
    endif()
endforeach()

if(unmirrored_headers)
    list(JOIN unmirrored_headers "\n" unmirrored_report)
    message(
        FATAL_ERROR
        "These headers have no mirrored test source:\n${unmirrored_report}\n"
        "Every public header is mirrored by exactly one test source; add it, "
        "or the mirror is no longer a rule."
    )
endif()

# The same headers are the interface line: none of them opens a detail namespace, so what a user never spells sits under detail/, where it is excepted from the mirror above.
set(detail_opening_headers)
foreach(h ${all_public_headers})
    file(STRINGS ${public_header_dir}/${h} detail_namespaces REGEX "^namespace [a-z:]*detail")
    if(detail_namespaces)
        list(APPEND detail_opening_headers "    include/${h}")
    endif()
endforeach()

if(detail_opening_headers)
    list(JOIN detail_opening_headers "\n" detail_opening_report)
    message(
        FATAL_ERROR
        "These public headers open a detail namespace:\n${detail_opening_report}\n"
        "Move what they define into a header under a detail/ directory and include it from there."
    )
endif()

if(NOT DEFINED Boost_USE_STATIC_LIBS)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(
    Boost 1.70 CONFIG REQUIRED COMPONENTS
    align
    unit_test_framework
)

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
    add_library(Boost::int128 INTERFACE IMPORTED)
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

# The hashing adapter's library, and not a hard dependency either. Boost ships it from 1.88, and the
# vcpkg manifest's test feature names it; it has too many Boost dependencies of its own to fetch alone.
find_package(boost_hash2 CONFIG QUIET)

if(TARGET Boost::hash2)
    set(${project_id}_test_boost_hash2 Boost::hash2)
else()
    set(${project_id}_test_boost_hash2)
endif()

# The headers held back above, each compiled only where the library it adapts is there to adapt.
if(TARGET Boost::int128 AND TARGET Boost::hash2)
    cmake_language(CALL ${project_id}_add_header_self_sufficiency_test xstd/ints/ext/boost.hpp ${${project_id}_test_boost_int128} ${${project_id}_test_boost_hash2})
endif()
if(TARGET Boost::int128)
    cmake_language(CALL ${project_id}_add_header_self_sufficiency_test xstd/ints/ext/boost/int128.hpp ${${project_id}_test_boost_int128})
endif()
if(TARGET Boost::hash2)
    cmake_language(CALL ${project_id}_add_header_self_sufficiency_test xstd/ints/ext/boost/hash2.hpp ${${project_id}_test_boost_hash2})
endif()
if(TARGET absl::int128)
    cmake_language(CALL ${project_id}_add_header_self_sufficiency_test xstd/ints/ext/absl.hpp ${${project_id}_test_absl_int128})
    cmake_language(CALL ${project_id}_add_header_self_sufficiency_test xstd/ints/ext/absl/int128.hpp ${${project_id}_test_absl_int128})
endif()

# Each C++ example in the README is compiled as its own translation unit, so the documentation cannot drift from the
# code: readme.example<n> is the README's n-th ```cpp block, with an empty main appended.
set(readme ${PROJECT_SOURCE_DIR}/README.md)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${readme})
file(READ ${readme} readme_text)
# A Windows checkout may carry CRLF line endings, which the fences below are matched without.
string(REPLACE "\r\n" "\n" readme_text "${readme_text}")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/readme)
set(readme_example 0)
while(TRUE)
    string(FIND "${readme_text}" "\n```cpp\n" fence)
    if(fence EQUAL -1)
        break()
    endif()
    math(EXPR fence "${fence} + 8")
    string(SUBSTRING "${readme_text}" ${fence} -1 readme_text)
    string(FIND "${readme_text}" "\n```\n" fence)
    if(fence EQUAL -1)
        message(FATAL_ERROR "README.md: a ```cpp block is never closed")
    endif()
    math(EXPR fence "${fence} + 1")
    string(SUBSTRING "${readme_text}" 0 ${fence} example_code)
    string(SUBSTRING "${readme_text}" ${fence} -1 readme_text)
    math(EXPR readme_example "${readme_example} + 1")

    # An example naming an <xstd/ints/ext/> header, or the library behind one, builds only where that library resolved.
    set(example_libraries)
    if(example_code MATCHES "<xstd/ints/ext/boost([.]hpp|/int128[.]hpp)>|<boost/int128")
        list(APPEND example_libraries Boost::int128)
    endif()
    if(example_code MATCHES "<xstd/ints/ext/boost([.]hpp|/hash2[.]hpp)>|<boost/hash2/")
        list(APPEND example_libraries Boost::hash2)
    endif()
    if(example_code MATCHES "<xstd/ints/ext/absl([.]hpp|/int128[.]hpp)>|<absl/")
        list(APPEND example_libraries absl::int128)
    endif()
    set(example_unresolved)
    foreach(library ${example_libraries})
        if(NOT TARGET ${library})
            list(APPEND example_unresolved ${library})
        endif()
    endforeach()
    if(example_unresolved)
        message(STATUS "readme.example${readme_example} is not built: ${example_unresolved} did not resolve")
        continue()
    endif()

    set(example_source ${CMAKE_CURRENT_BINARY_DIR}/readme/example${readme_example}.cpp)
    file(WRITE ${example_source} "${example_code}\nauto main() -> int {}\n")
    add_executable(readme.example${readme_example} ${example_source})
    target_link_libraries(
        readme.example${readme_example} PRIVATE
        xstd::ints
        ${example_libraries}
        ${project_id}_test_options
    )
    # An example declares its variables at namespace scope, where a reader would declare them in a function.
    target_compile_options(
        readme.example${readme_example} PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wno-missing-variable-declarations -Wno-exit-time-destructors -Wno-unused-const-variable>
    )
    add_test(NAME readme.example${readme_example} COMMAND readme.example${readme_example})
endwhile()
if(readme_example EQUAL 0)
    message(FATAL_ERROR "README.md has no ```cpp block, so no example of it is compiled")
endif()

set(current_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(
    GLOB_RECURSE targets
    CONFIGURE_DEPENDS
    RELATIVE ${current_source_dir}
    ${current_source_dir}/*.cpp
)
foreach(t ${targets})
    cmake_path(GET t PARENT_PATH target_path)
    cmake_path(GET t STEM target_name_we)
    string(REPLACE "/" "." target_id ${target_path}/${target_name_we})
    string(REGEX REPLACE "^[.]" "" target_id ${target_id})

    add_executable(${target_id} src/${t})

    target_link_libraries(
        ${target_id} PRIVATE
        xstd::ints
        Boost::unit_test_framework
        ${${project_id}_test_boost_int128}
        ${${project_id}_test_boost_hash2}
        ${${project_id}_test_absl_int128}
        ${project_id}_test_options
    )

    add_test(NAME ${target_id} COMMAND ${target_id} --log_level=all)
endforeach()

# Each source under death/ calls one function once with an argument its precondition refuses, and
# passes only by failing that assert, which death/expect_assert.cmake checks as its test launcher.
# They are registered only where assert is live, so where neither CMAKE_CXX_FLAGS nor the
# build type's own flags define NDEBUG, and not under a multi-config generator, whose build type is
# chosen after configuring. Not on Windows either, where a failed assert in the debug runtime opens a
# dialog that no CI runner dismisses.
get_property(${project_id}_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
string(TOUPPER "${CMAKE_BUILD_TYPE}" build_type_id)
if(NOT WIN32 AND NOT ${project_id}_is_multi_config AND NOT "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${build_type_id}}" MATCHES "NDEBUG")
    set(death_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/death)
    file(
        GLOB_RECURSE death_sources
        CONFIGURE_DEPENDS
        RELATIVE ${death_source_dir}
        ${death_source_dir}/*.cpp
    )
    foreach(d ${death_sources})
        # A case over Boost.Int128, the one type outside <bit> and std::to_chars on every platform, needs it resolved.
        file(STRINGS ${death_source_dir}/${d} death_needs_boost_int128 REGEX "#include[ \t]*<boost/int128")
        if(death_needs_boost_int128 AND NOT TARGET Boost::int128)
            continue()
        endif()

        # death/ints/cstdlib/div/zero_denominator.cpp is death.ints.cstdlib.div.zero_denominator.
        string(REGEX REPLACE "[.]cpp$" "" death_id ${d})
        string(REPLACE "/" "." death_id death.${death_id})

        add_executable(${death_id} death/${d})
        target_link_libraries(
            ${death_id} PRIVATE
            xstd::ints
            ${${project_id}_test_boost_int128}
            ${project_id}_test_options
        )

        # The launcher comes before the cross-compiling emulator, so the script runs whatever the test would.
        set_target_properties(${death_id} PROPERTIES TEST_LAUNCHER "${CMAKE_COMMAND};-P;${death_source_dir}/expect_assert.cmake;--")
        add_test(NAME ${death_id} COMMAND ${death_id})
    endforeach()
endif()
