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
