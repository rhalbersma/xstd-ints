#!/usr/bin/env bash
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Prints the path of a cpp-ci checkout at the one commit this repository's workflows pin, fetching it once into
# ${XDG_CACHE_HOME:-~/.cache}/cpp-ci/<commit>. setup-toolchain.sh and ci-local.sh run cpp-ci's own tools from it, so
# a local run installs and builds with what CI's pinned workflows do. CPP_CI_DIR names a checkout of your own instead,
# such as a cpp-ci branch under test.
#
#   tools/cpp-ci.sh

set -euo pipefail

if [ -n "${CPP_CI_DIR:-}" ]; then
        cd "${CPP_CI_DIR}" && pwd
        exit 0
fi

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
mapfile -t pins < <(grep -rhoE 'rhalbersma/cpp-ci/\.github/(workflows|actions)/[^@[:space:]]+@[0-9a-f]{40}' "${root}/.github/workflows" \
        | sed 's/.*@//' | sort -u)
if [ "${#pins[@]}" -ne 1 ]; then
        echo "error: .github/workflows pins cpp-ci at ${#pins[@]} commits, not one: ${pins[*]:-none}" >&2
        exit 1
fi
commit="${pins[0]}"
checkout="${XDG_CACHE_HOME:-${HOME:-/root}/.cache}/cpp-ci/${commit}"
if [ "$(git -C "${checkout}" rev-parse HEAD 2>/dev/null)" != "${commit}" ]; then
        rm -rf "${checkout}"
        mkdir -p "${checkout}"
        git -C "${checkout}" init -q
        git -C "${checkout}" fetch -q --depth 1 https://github.com/rhalbersma/cpp-ci "${commit}"
        git -C "${checkout}" checkout -q FETCH_HEAD
fi
echo "${checkout}"
