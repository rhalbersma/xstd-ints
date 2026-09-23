#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.30)
project(
    xstd-ints
    VERSION 0.1.0
    DESCRIPTION "Do as the ints do"
    # Spelled out rather than derived from ${PROJECT_NAME}: project() expands its
    # own arguments before it sets that variable, so a derived URL would silently
    # record https://github.com/rhalbersma/ with no repository on the end.
    HOMEPAGE_URL "https://github.com/rhalbersma/xstd-ints"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# The strictly conforming dialect, for the library and anything a consumer builds
# against it. Set before any target; consumers keep control over their own. The test
# matrix relaxes it below, and only there.
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
    ${PROJECT_NAME} INTERFACE
)

# Consumed as xstd::ints, following Boost: the package keeps the repository's name
# (find_package(xstd-ints)) while the target is a component of one umbrella namespace,
# so xstd::ints sits beside xstd::bits the way Boost::mp11 sits beside Boost::hash2.
# EXPORT_NAME is what lets the exported name differ from the target name; the alias
# below must spell the same thing, or an add_subdirectory consumer and a find_package
# consumer would end up with two different names for one library.
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME ints)
add_library(xstd::ints ALIAS ${PROJECT_NAME})

# No target_include_directories: the HEADERS file set below adds its base
# directory, and install(TARGETS ... FILE_SET) adds the install destination.
target_compile_features(
    ${PROJECT_NAME} INTERFACE
    cxx_std_23
)

target_sources(
    ${PROJECT_NAME} INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/xstd/ints.hpp
        include/xstd/ints/bit.hpp
        include/xstd/ints/bit/countl_zero.hpp
        include/xstd/ints/bit/countr_zero.hpp
        include/xstd/ints/bit/popcount.hpp
        include/xstd/ints/charconv.hpp
        include/xstd/ints/charconv/to_chars.hpp
        include/xstd/ints/concepts.hpp
        include/xstd/ints/concepts/alignable.hpp
        include/xstd/ints/concepts/bitwise_operators.hpp
        include/xstd/ints/concepts/integer.hpp
        include/xstd/ints/concepts/integer_class.hpp
        include/xstd/ints/concepts/nothrow_const_operators.hpp
        include/xstd/ints/concepts/signed_integer.hpp
        include/xstd/ints/concepts/unsigned_integer.hpp
        include/xstd/ints/cstdint.hpp
        include/xstd/ints/cstdint/bit_int.hpp
        include/xstd/ints/cstdint/int128.hpp
        include/xstd/ints/cstdlib.hpp
        include/xstd/ints/cstdlib/abs.hpp
        include/xstd/ints/cstdlib/div.hpp
        include/xstd/ints/cstdlib/div_euclid.hpp
        include/xstd/ints/cstdlib/div_floor.hpp
        include/xstd/ints/cstdlib/div_result.hpp
        include/xstd/ints/cstdlib/sign.hpp
        include/xstd/ints/cstdlib/unsigned_abs.hpp
        include/xstd/ints/detail/delegates_to_std.hpp
        include/xstd/ints/ext/absl.hpp
        include/xstd/ints/ext/absl/int128.hpp
        include/xstd/ints/ext/boost.hpp
        include/xstd/ints/ext/boost/int128.hpp
        include/xstd/ints/format.hpp
        include/xstd/ints/format/div_result.hpp
        include/xstd/ints/limits.hpp
        include/xstd/ints/limits/numeric_limits.hpp
        include/xstd/ints/memory.hpp
        include/xstd/ints/memory/align_down.hpp
        include/xstd/ints/memory/align_up.hpp
        include/xstd/ints/memory/is_aligned.hpp
        include/xstd/ints/type_traits.hpp
        include/xstd/ints/type_traits/is_signed.hpp
        include/xstd/ints/type_traits/is_unsigned.hpp
        include/xstd/ints/type_traits/make_signed.hpp
        include/xstd/ints/type_traits/make_unsigned.hpp
        include/xstd/ints/type_traits/promoted.hpp
)

# Tests and their Boost.Test dependency only exist when xstd-ints is being
# developed; a consumer vendoring this directory gets the header-only target.
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        # Relaxed to gnu++ for the tests alone, leaving the library's own dialect above
        # untouched. The reason is specific: <bit>'s functions take std::unsigned_integral,
        # and libstdc++ and libc++ carry unsigned __int128 into that concept only outside
        # __STRICT_ANSI__. Strictly conforming, xstd::uint128 is the one exact width with no
        # bit basis on GCC and Clang -- on MSVC it is a class and carries its own -- so the
        # 128-bit column would drop out of the bit/ instantiation lists rather than fail,
        # which is the worse of the two outcomes. xstd-bits relaxes it in the same place and
        # for the same reason. [xstd-bits design.md#uint128-support]
        set(CMAKE_CXX_EXTENSIONS ON)

        add_subdirectory(test)
    endif()
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
)
install(
    EXPORT ${PROJECT_NAME}Targets
    NAMESPACE xstd::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# So consumers can find_package(xstd-ints CONFIG). ARCH_INDEPENDENT because a
# header-only library has no architecture-specific binaries.
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    COMPATIBILITY SameMinorVersion
    ARCH_INDEPENDENT
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
