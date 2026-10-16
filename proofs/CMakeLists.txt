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

# Both std::size_t widths, whatever the host's own: the proofs are about the kernels, not this build of them.
add_test(NAME proofs.z3 COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/prove.py --widths 32,64)

set_tests_properties(proofs.drift proofs.z3 PROPERTIES TIMEOUT 7200)
