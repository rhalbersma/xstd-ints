#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

import os

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.files import copy, get
from conan.tools.layout import basic_layout
from conan.tools.scm import Version

required_conan_version = ">=2.0"


class XstdIntsConan(ConanFile):
    name = "xstd-ints"
    description = "Do as the ints do: <bit>, <cstdlib>, <charconv> and <memory> integer functions for every integer type, the 128-bit and bit-precise ones included."
    license = "BSL-1.0"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/rhalbersma/xstd-ints"
    topics = ("integer", "bit-manipulation", "int128", "bitint", "header-only")
    package_type = "header-library"
    settings = "os", "arch", "compiler", "build_type"
    no_copy_source = True

    # The oldest of each the library's CI builds with.
    _minimum_compilers = {
        "gcc": "15",
        "clang": "20",
        "apple-clang": "17",
        "msvc": "194",
    }

    def layout(self):
        basic_layout(self, src_folder="src")

    def package_id(self):
        self.info.clear()

    def validate(self):
        check_min_cppstd(self, 23)
        minimum = self._minimum_compilers.get(str(self.settings.compiler))
        if minimum and Version(self.settings.compiler.version) < minimum:
            raise ConanInvalidConfiguration(f"{self.ref} requires {self.settings.compiler} {minimum} or later")

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def build(self):
        pass

    def package(self):
        copy(self, "LICENSE_1_0.txt", self.source_folder, os.path.join(self.package_folder, "licenses"))
        copy(self, "*.hpp", os.path.join(self.source_folder, "include"), os.path.join(self.package_folder, "include"))

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        self.cpp_info.set_property("cmake_file_name", "xstd-ints")
        self.cpp_info.set_property("cmake_target_name", "xstd::ints")
