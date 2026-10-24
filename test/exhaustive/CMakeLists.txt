#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

find_package(Threads REQUIRED)

foreach(target_id bit_permutation division)
    add_executable(exhaustive_${target_id} ${target_id}.cpp)

    # The bit sweep checks the properties the unit tests and the fuzzer check, from the test tree's header-only helpers.
    target_include_directories(
        exhaustive_${target_id} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/test/include
    )

    target_link_libraries(
        exhaustive_${target_id} PRIVATE
        xstd::ints
        Threads::Threads
    )

    target_compile_options(
        exhaustive_${target_id} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
    )
endforeach()

# Minutes rather than seconds each: 2^32 pairs, every one checked several ways.
add_test(NAME exhaustive.bit_permutation COMMAND exhaustive_bit_permutation)
add_test(NAME exhaustive.division COMMAND exhaustive_division ${CMAKE_CURRENT_SOURCE_DIR}/division_digests.txt)
set_tests_properties(exhaustive.bit_permutation exhaustive.division PROPERTIES TIMEOUT 3600)

include(FetchContent)

# Implementations xstd did not write, each pinned by commit and read as headers only: SOURCE_SUBDIR names no directory.
FetchContent_Declare(
    simde
    GIT_REPOSITORY https://github.com/simd-everywhere/simde.git
    GIT_TAG 2e5224981e9123b8fe89a9833a978556d636a565
    SOURCE_SUBDIR headers-only
)
FetchContent_Declare(
    multiprecision
    GIT_REPOSITORY https://github.com/boostorg/multiprecision.git
    GIT_TAG 529dfac199191a7eb8a5eb7f47256eff6d0db993 # boost-1.90.0
    SOURCE_SUBDIR headers-only
)
# Boost.Multiprecision's standalone mode still includes <boost/config.hpp>, which a runner without Boost lacks.
FetchContent_Declare(
    boost_config
    GIT_REPOSITORY https://github.com/boostorg/config.git
    GIT_TAG a7d5a9b05d70c9cfea980dc3539ca3d3461411b3 # boost-1.90.0
    SOURCE_SUBDIR headers-only
)
FetchContent_MakeAvailable(simde multiprecision boost_config)

add_executable(exhaustive_differential differential.cpp)
target_include_directories(exhaustive_differential PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(
    exhaustive_differential SYSTEM PRIVATE
    ${simde_SOURCE_DIR}
    ${multiprecision_SOURCE_DIR}/include
    ${boost_config_SOURCE_DIR}/include
)
target_link_libraries(
    exhaustive_differential PRIVATE
    xstd::ints
    Threads::Threads
)
# SIMDe's portable code rather than the instruction it stands for; Boost.Multiprecision without the rest of Boost.
target_compile_definitions(
    exhaustive_differential PRIVATE
    SIMDE_NO_NATIVE
    BOOST_MP_STANDALONE
)
target_compile_options(
    exhaustive_differential PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
)

# sux's headers need GCC or Clang builtins.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    FetchContent_Declare(
        sux
        GIT_REPOSITORY https://github.com/vigna/sux.git
        GIT_TAG 568903f1b7957ef03620ebad65d6ef68e031fef9
        SOURCE_SUBDIR headers-only
    )
    FetchContent_MakeAvailable(sux)
    target_include_directories(exhaustive_differential SYSTEM PRIVATE ${sux_SOURCE_DIR})
endif()

add_test(NAME exhaustive.differential COMMAND exhaustive_differential)
set_tests_properties(exhaustive.differential PROPERTIES TIMEOUT 3600)
