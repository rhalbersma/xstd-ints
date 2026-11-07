#!/usr/bin/env bash
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Brings a fresh Ubuntu machine, such as a Claude Code cloud container, as close to the GitHub Actions ladders as it
# gets, through cpp-ci's tools/setup-toolchain.sh at the commit this repository's workflows pin. That script installs
# every rung of the README's matrix from cpp-ci's toolchain table, with clang-tidy for each clang, libc++, GCC's
# 32-bit multilib, gcovr, actionlint and vcpkg; this one adds what is this repository's own:
#
#   the unversioned clang-format, FileCheck and the CMake the workflows pin, each from PyPI by hash;
#   Boost 1.92's Align, Hash2 and Test, the ones vcpkg.json asks for, built from Boost's release tarball into
#   /usr/local, which tools/ci-local.sh builds against.
#
#   tools/setup-toolchain.sh             every rung
#   tools/setup-toolchain.sh --stable    the stable rung alone, without Boost, for a CI job that builds on that rung
#
# It is idempotent: a second run installs nothing and finishes in seconds.

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
case "${1:-}" in
        "") boost=(--boost align,hash2,test) ;;
        --stable) boost=() ;;
        *) echo "usage: $0 [--stable]" >&2; exit 2 ;;
esac
cpp_ci="$("${root}/tools/cpp-ci.sh")"
exec "${cpp_ci}/tools/setup-toolchain.sh" "$@" \
        --requirements "${root}/tools/clang-format-requirements.txt" \
        --requirements "${root}/tools/filecheck-requirements.txt" \
        --requirements "${root}/tools/cmake-requirements.txt" \
        "${boost[@]}"
