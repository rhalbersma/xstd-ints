#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include(CheckCXXSourceCompiles)

find_program(VALGRIND_EXECUTABLE valgrind REQUIRED)
find_path(VALGRIND_INCLUDE_DIR valgrind/memcheck.h REQUIRED)

set(CMAKE_REQUIRED_FLAGS "${CMAKE_CXX_FLAGS}")
check_cxx_source_compiles("#ifndef __BMI2__\n#error\n#endif\nint main() {}" constant_time_has_bmi2)
unset(CMAKE_REQUIRED_FLAGS)

add_executable(constant_time_memcheck memcheck.cpp)
target_include_directories(constant_time_memcheck PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${VALGRIND_INCLUDE_DIR})
target_link_libraries(constant_time_memcheck PRIVATE xstd::ints)
target_compile_options(constant_time_memcheck PRIVATE -Wall -Wextra)

# Each case names a kernel and the argument kept secret. A case that memcheck flags is expected to fail: its kernel
# branches on the secret or indexes memory by it, so the test pins what the kernel does rather than what it ought to.
set(clean_cases reverse_by_swaps.word expand_by_walk.source)
set(flagged_cases select_by_broadword.dense select_by_broadword.one_bit expand_by_walk.mask compress_by_rank.source expand_by_select.source)
if(constant_time_has_bmi2)
    list(APPEND clean_cases pext.source pext.mask pdep.mask)
endif()

foreach(case IN LISTS clean_cases flagged_cases)
    add_test(NAME constant_time.memcheck.${case} COMMAND ${VALGRIND_EXECUTABLE} --quiet --error-exitcode=1 $<TARGET_FILE:constant_time_memcheck> ${case})
endforeach()
foreach(case IN LISTS flagged_cases)
    set_tests_properties(constant_time.memcheck.${case} PROPERTIES WILL_FAIL TRUE)
endforeach()

# dudect times with RDTSC, so its harness is x86-64 only; it is run by hand, being a statistical test without a verdict
# of its own when it finds nothing.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    include(FetchContent)
    FetchContent_Declare(
        dudect
        GIT_REPOSITORY https://github.com/oreparaz/dudect.git
        GIT_TAG dc269651fb2567e46755cfb2a13d3875592968b5
    )
    FetchContent_MakeAvailable(dudect)
    enable_language(C)
    add_executable(constant_time_dudect dudect.cpp dudect_implementation.c)
    target_include_directories(constant_time_dudect PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${dudect_SOURCE_DIR}/src)
    target_link_libraries(constant_time_dudect PRIVATE xstd::ints m)
endif()
