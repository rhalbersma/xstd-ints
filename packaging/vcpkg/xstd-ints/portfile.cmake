vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO rhalbersma/xstd-ints
    REF "v${VERSION}"
    SHA512 8702dfa178261e8b0ab9cd352af720e745c4841d065c051e900bfbcb70bf5c51789e93f394f1a4c12bef99497e175fee0ee715502f43001b685032f4bb95c4fe
    HEAD_REF main
)

set(VCPKG_BUILD_TYPE release) # header-only

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_TESTING=OFF
)
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/${PORT})

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/lib")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE_1_0.txt")
