#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Mull mutates LLVM IR through a clang pass plugin built against one LLVM major, so the compiler is the clang of the
# Mull package installed beside it.
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    message(FATAL_ERROR "${project_option_prefix}_BUILD_MUTATION_TESTS needs clang with its GNU-style driver; this is ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}.")
endif()

string(REGEX MATCH "^[0-9]+" llvm_major ${CMAKE_CXX_COMPILER_VERSION})

find_file(
    ${project_option_prefix}_MULL_FRONTEND
    NAMES mull-ir-frontend-${llvm_major}
    PATHS /usr/lib /usr/local/lib
    NO_DEFAULT_PATH
    REQUIRED
)
find_program(
    ${project_option_prefix}_MULL_RUNNER
    NAMES mull-runner-${llvm_major}
    REQUIRED
)
find_package(Python3 3.10 COMPONENTS Interpreter REQUIRED)

if(NOT DEFINED Boost_USE_STATIC_LIBS)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(Boost 1.70 CONFIG REQUIRED COMPONENTS unit_test_framework)

# gnu++ where the unit tests are: there <bit> carries unsigned __int128 into std::unsigned_integral only outside
# __STRICT_ANSI__, and the 128-bit column would otherwise drop out of the instantiation lists.
set(CMAKE_CXX_EXTENSIONS ${${project_option_prefix}_STRICT_DROPS_UINT128})

set(mull_config ${CMAKE_CURRENT_SOURCE_DIR}/mull.yml)
set(test_source_dir ${PROJECT_SOURCE_DIR}/test/src)

# The unit tests that reach the mutated headers, one executable each, so that a mutant reruns only the tests compiled
# with it. The intrinsic kernels' test is left out: it reaches no mutated line without BMI2, and is the slowest of all.
file(
    GLOB_RECURSE sources
    CONFIGURE_DEPENDS
    RELATIVE ${test_source_dir}
    ${test_source_dir}/ints/bit/*.cpp
    ${test_source_dir}/ints/cstdlib/*.cpp
)
list(REMOVE_ITEM sources ints/bit/detail/intrinsic.cpp)

set(executables)
foreach(source ${sources})
    cmake_path(REMOVE_EXTENSION source OUTPUT_VARIABLE stem)
    string(REPLACE "/" "." id ${stem})
    set(target mutation.${id})

    add_executable(${target} ${test_source_dir}/${source})

    target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR}/test/include)
    target_compile_definitions(
        ${target} PRIVATE
        BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
        BOOST_ALL_NO_LIB
        BOOST_TEST_MAIN
    )
    # Unoptimized, so that every call a test makes reaches the mutated code instead of a value folded at compile time.
    target_compile_options(
        ${target} PRIVATE
        -O0
        -g
        -grecord-command-line
        -fpass-plugin=${${project_option_prefix}_MULL_FRONTEND}
    )
    target_link_libraries(${target} PRIVATE xstd::ints Boost::unit_test_framework)

    # The plugin finds its configuration by walking up from the working directory, which a build tree need not share.
    set_target_properties(
        ${target} PROPERTIES
        CXX_COMPILER_LAUNCHER "${CMAKE_COMMAND};-E;env;MULL_CONFIG=${mull_config}"
    )

    list(APPEND executables $<TARGET_FILE:${target}>)
endforeach()

add_test(
    NAME mutation.score
    COMMAND
        Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/score.py
        --runner ${${project_option_prefix}_MULL_RUNNER}
        --config ${mull_config}
        --report-dir ${CMAKE_CURRENT_BINARY_DIR}/reports
        ${executables}
)
