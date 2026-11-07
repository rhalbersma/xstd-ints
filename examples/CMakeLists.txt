#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The examples written for the reference. Each <group>/<page>.cpp is the Example section of doc/reference/<group>/<page>.md:
# built with the tests' warnings, run, and what it prints compared with the <page>.out beside it, which the page shows as
# its Output. perft/ is a program of its own, built behind XSTD_INTS_BUILD_EXAMPLES, and is not one of them.
file(
    GLOB examples
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp
)
list(FILTER examples EXCLUDE REGEX "^perft/")

foreach(e ${examples})
    string(REGEX REPLACE "[.]cpp$" "" stem ${e})
    string(REPLACE "/" "." example_id ${stem})
    set(expected ${CMAKE_CURRENT_SOURCE_DIR}/${stem}.out)
    if(NOT EXISTS ${expected})
        message(FATAL_ERROR "examples/${e} has no examples/${stem}.out: run it and save what it prints there")
    endif()

    # An example of an <xstd/ints/ext/> header builds only where the library behind it resolved, as the README's do.
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/${e} example_code)
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
        message(STATUS "example.${example_id} is not built: ${example_unresolved} did not resolve")
        continue()
    endif()

    add_executable(example.${example_id} ${e})
    target_link_libraries(
        example.${example_id} PRIVATE
        xstd::ints
        ${example_libraries}
        ${project_id}_test_options
    )
    add_test(
        NAME example.${example_id}
        COMMAND ${CMAKE_COMMAND}
            -Dexample=$<TARGET_FILE:example.${example_id}>
            -Demulator=$<TARGET_PROPERTY:example.${example_id},CROSSCOMPILING_EMULATOR>
            -Dexpected=${expected}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/check_output.cmake
    )
endforeach()

if(NOT examples)
    message(FATAL_ERROR "examples/ holds no example, so no reference page has one")
endif()
