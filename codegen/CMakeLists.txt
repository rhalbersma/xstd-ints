#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# The expectations are written against GCC's and Clang's assembly, in Intel syntax on x86-64.
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    message(FATAL_ERROR "${project_option_prefix}_BUILD_CODEGEN_TESTS needs GCC or clang with its GNU-style driver; this is ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}.")
endif()

# The Python port that tools/filecheck-requirements.txt pins, or LLVM's own, which reads the same directives.
find_program(
    ${project_option_prefix}_FILECHECK
    NAMES filecheck FileCheck
    HINTS $ENV{HOME}/.local/bin
    REQUIRED
)

# What a consumer builds: the strictly conforming dialect, optimized, with the library's asserts compiled out.
set(common_flags "-std=c++23 -O2 -DNDEBUG -I${PROJECT_SOURCE_DIR}/include")

# One test per source and target configuration. Every configuration reads CHECK, which labels each function.
function(add_codegen_test source config flags prefixes)
    cmake_parse_arguments(PARSE_ARGV 4 arg "" "EXPECT;SAME_BODIES" "")
    get_filename_component(stem ${source} NAME_WE)
    set(name ${stem}.${config})
    add_test(
        NAME codegen.${name}
        COMMAND
            ${CMAKE_COMMAND}
            -DCXX=${CMAKE_CXX_COMPILER}
            "-DFLAGS=${common_flags} ${flags}"
            -DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/${source}
            -DASM=${CMAKE_CURRENT_BINARY_DIR}/${name}.s
            -DFILECHECK=${${project_option_prefix}_FILECHECK}
            -DPREFIXES=${prefixes}
            -DEXPECT=${arg_EXPECT}
            -DSAME_BODIES=${arg_SAME_BODIES}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.cmake
    )
endfunction()

set(alignment_pairs "codegen_align_up=codegen_align_up_by_complement,codegen_align_down=codegen_align_down_by_complement")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
    add_codegen_test(bit_permutation.cpp x86-64 "-masm=intel -march=x86-64" "CHECK,X86-STRAIGHT,PORTABLE")
    add_codegen_test(bit_permutation.cpp bmi2 "-masm=intel -mbmi2" "CHECK,X86-STRAIGHT,FAST-BMI2,PDEP-SELECT")
    add_codegen_test(bit_permutation.cpp x86-64-v3 "-masm=intel -march=x86-64-v3" "CHECK,X86-STRAIGHT,FAST-BMI2,TZCNT")
    add_codegen_test(bit_permutation.cpp gfni "-masm=intel -mgfni" "CHECK,X86-STRAIGHT,PORTABLE,GFNI")

    # BMI2 is enabled on each of these, but PEXT and PDEP are microcoded there.
    foreach(march bdver4 znver1 znver2)
        add_codegen_test(bit_permutation.cpp ${march} "-masm=intel -march=${march}" "CHECK,X86-STRAIGHT,PORTABLE")
    endforeach()

    # BMI2 enabled and its kernels excluded: the checks for the instructions must see them gone.
    add_codegen_test(bit_permutation.cpp bmi2-excluded "-masm=intel -march=znver2" "CHECK,FAST-BMI2,TZCNT" EXPECT mismatch)

    add_codegen_test(alignment.cpp x86-64 "-masm=intel -march=x86-64" "" SAME_BODIES ${alignment_pairs})
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
    add_codegen_test(bit_permutation.cpp armv8-a "-march=armv8-a" "CHECK,A64")
    add_codegen_test(alignment.cpp armv8-a "-march=armv8-a" "" SAME_BODIES ${alignment_pairs})
else()
    message(FATAL_ERROR "${project_option_prefix}_BUILD_CODEGEN_TESTS has expectations for x86-64 and AArch64; this is ${CMAKE_SYSTEM_PROCESSOR}.")
endif()
