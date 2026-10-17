#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# A clean project that knows xstd-ints only as the port in its manifest, running Conan's test program.
cmake_minimum_required(VERSION 3.15)
project(test_port LANGUAGES CXX)

find_package(xstd-ints CONFIG REQUIRED)

add_executable(test_port ../../conan/test_package/test_package.cpp)
target_link_libraries(test_port PRIVATE xstd::ints)
target_compile_features(test_port PRIVATE cxx_std_23)

enable_testing()
add_test(NAME test_port COMMAND test_port)
