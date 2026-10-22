#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The proofs and the drift check are Python, with Z3's bindings from tools/z3-requirements.txt.
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# The compiled portable kernels, for the drift check to hold the models against.
add_executable(proofs_drift drift.cpp)
target_link_libraries(proofs_drift PRIVATE xstd::ints)
target_compile_options(
    proofs_drift PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra>
)

add_test(NAME proofs.drift COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/drift.py $<TARGET_FILE:proofs_drift>)

# The same kernels again at a 32-bit std::size_t, so the 32-bit models meet a build of their own width and not only
# the proofs. A -m32 build needs a multilib toolchain; ON requires one, AUTO uses one where the probe links.
set(${project_option_prefix}_PROOFS_DRIFT_32 AUTO CACHE STRING "Hold the 32-bit models to a -m32 build of the kernels: ON, OFF or AUTO")
set_property(CACHE ${project_option_prefix}_PROOFS_DRIFT_32 PROPERTY STRINGS ON OFF AUTO)

if(NOT ${project_option_prefix}_PROOFS_DRIFT_32 STREQUAL "OFF" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_FLAGS -m32)
    set(CMAKE_REQUIRED_LINK_OPTIONS -m32)
    check_cxx_source_compiles("#include <iostream>\nint main() { std::cout << sizeof(void*); }" proofs_links_m32)
    unset(CMAKE_REQUIRED_FLAGS)
    unset(CMAKE_REQUIRED_LINK_OPTIONS)
endif()

if(proofs_links_m32)
    add_executable(proofs_drift_32 drift.cpp)
    target_link_libraries(proofs_drift_32 PRIVATE xstd::ints)
    target_compile_options(proofs_drift_32 PRIVATE -m32 -Wall -Wextra)
    target_link_options(proofs_drift_32 PRIVATE -m32)
    add_test(NAME proofs.drift_32 COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/drift.py $<TARGET_FILE:proofs_drift_32>)
    set_tests_properties(proofs.drift_32 PROPERTIES TIMEOUT 7200)
elseif(${project_option_prefix}_PROOFS_DRIFT_32 STREQUAL "ON")
    message(FATAL_ERROR "${project_option_prefix}_PROOFS_DRIFT_32 is ON, but ${CMAKE_CXX_COMPILER} cannot link a -m32 program; install its multilib, such as g++-15-multilib.")
endif()

# Both std::size_t widths, whatever the host's own: the proofs are about the kernels, not this build of them.
add_test(NAME proofs.z3 COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/prove.py --widths 32,64)

set_tests_properties(proofs.drift proofs.z3 PROPERTIES TIMEOUT 7200)
