#!/usr/bin/env bash
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Runs the GitHub Actions jobs' Linux legs on what setup-toolchain.sh installs, so a change fails here, fast, before
# it waits on CI's runners. The legs this repository's workflow stubs take from cpp-ci run through cpp-ci's
# tools/ci_local.py, at the commit the stubs pin, which reads them from the stubs themselves; the jobs this repository
# defines itself, layering, site, codegen and std-test-suite, run here beside them. Legs run cheapest first and the
# run stops at the first that fails. Each leg works under build/ci-local/<leg> and logs to build/ci-local/<leg>.log,
# and the run ends in one line per leg.
#
#   tools/ci-local.sh --quick                before a commit: actionlint, format, layering, GCC 15, clang 22, clang-tidy 22
#   tools/ci-local.sh                        before a push or a pull request: every leg
#   tools/ci-local.sh gcc-17-svn clang-tidy-24-svn   the legs named, in the order given
#   tools/ci-local.sh --keep-going ...       every leg asked for, whatever fails
#   tools/ci-local.sh --list                 the legs, in the order a full run takes them
#
# CI_LOCAL_DEPS=vcpkg builds the dependencies through vcpkg, as CI does; the default, system, builds against the Boost
# setup-toolchain.sh installs, for a machine whose network refuses the source archives vcpkg downloads. CPP_CI_DIR runs
# a cpp-ci checkout of your own in place of the pinned one.
#
# Not here, having no Linux equivalent: MSVC, clang-cl, MinGW and Apple Clang. Emscripten, CodeQL, s390x and the
# libc++ and sanitizer legs of the rungs above the stable one are CI's too; --list names each with its reason.

set -uo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
readonly root
readonly out="${root}/build/ci-local"
readonly deps="${CI_LOCAL_DEPS:-system}"
readonly jobs="$(nproc)"
readonly local_bin="${HOME:-/root}/.local/bin"
cmake="${local_bin}/cmake"
[ -x "${cmake}" ] || cmake="cmake"
readonly cmake
cpp_ci="$("${root}/tools/cpp-ci.sh")" || exit 1
readonly cpp_ci
ci_local=(python3 -I "${cpp_ci}/tools/ci_local.py" --caller "${root}" --out "${out}" --deps "${deps}")

# This repository's own legs: the cheap checks after format, the stable rung's builds after its suite legs.
readonly own_checks=(layering site)
readonly own_builds=(codegen std-test-suite)
readonly quick_legs=(actionlint format layering gcc-15 clang-22 clang-tidy-22)

# cpp-ci's layering check, from the pinned checkout, over the folder kinds tools/layering.toml names.
layering() {
        (cd "${root}" && python3 -I "${cpp_ci}/.github/actions/layering/layering.py" tools/layering.toml)
}

# The Pages workflow's build: MkDocs Material, pinned by hash, into a virtual environment of its own, then the site in
# --strict mode, which fails on a link to a page or an anchor that does not exist.
site() {
        local dir="${out}/site"
        if [ ! -x "${dir}/venv/bin/mkdocs" ]; then
                python3 -m venv "${dir}/venv" \
                        && "${dir}/venv/bin/pip" install --quiet --require-hashes -r "${root}/tools/mkdocs-requirements.txt" || return 1
        fi
        (cd "${root}" && PYTHONDONTWRITEBYTECODE=1 "${dir}/venv/bin/mkdocs" build --strict --site-dir "${dir}/html")
}

# The codegen job's FileCheck tests, on both compilers it runs.
codegen() {
        local cxx dir status=0
        for cxx in g++-15 clang++-22; do
                dir="${out}/codegen-${cxx}"
                "${cmake}" -S "${root}" -B "${dir}" -G Ninja -DCMAKE_CXX_COMPILER="${cxx}" -DBUILD_TESTING=OFF \
                        -DXSTD_INTS_BUILD_CODEGEN_TESTS=ON \
                        && ctest --test-dir "${dir}" --output-on-failure || status=1
        done
        return "${status}"
}

# The std-test-suite job: libc++'s, libstdc++'s and GCC trunk's tests of what xstd mirrors, each on its own library.
# The MSVC STL's run beside the first two.
std_test_suite() {
        local dir="${out}/std-test-suite"
        "${cmake}" -S "${root}" -B "${dir}/libcxx" -G Ninja -DCMAKE_CXX_COMPILER=clang++-22 -DCMAKE_CXX_FLAGS=-stdlib=libc++ \
                -DCMAKE_EXE_LINKER_FLAGS=-stdlib=libc++ -DBUILD_TESTING=OFF -DXSTD_INTS_STD_TEST_SUITES=ON \
                && "${cmake}" --build "${dir}/libcxx" --parallel "${jobs}" \
                && ctest --test-dir "${dir}/libcxx" --output-on-failure --parallel "${jobs}" \
                && "${cmake}" -S "${root}" -B "${dir}/libstdcxx" -G Ninja -DCMAKE_CXX_COMPILER=g++-15 \
                        -DBUILD_TESTING=OFF -DXSTD_INTS_STD_TEST_SUITES=ON \
                && "${cmake}" --build "${dir}/libstdcxx" --parallel "${jobs}" \
                && ctest --test-dir "${dir}/libstdcxx" --output-on-failure --parallel "${jobs}" \
                && "${cmake}" -S "${root}" -B "${dir}/libstdcxx-trunk" -G Ninja -DCMAKE_CXX_COMPILER=g++-17 -DCMAKE_CXX_FLAGS=-std=gnu++2d \
                        -DBUILD_TESTING=OFF -DXSTD_INTS_STD_TEST_SUITES=ON -DXSTD_INTS_STD_TEST_SUITE_RUNG=development \
                && "${cmake}" --build "${dir}/libstdcxx-trunk" --parallel "${jobs}"
}

# cpp-ci's legs in its run order, with this repository's own woven in: the checks after format, the builds after the
# last leg on a stable-rung compiler before any other rung's.
all_legs() {
        local stable
        stable="$(python3 -I -c 'import json, sys; t = json.load(open(sys.argv[1])); print(t["gcc"]["stable"]["version"], t["clang"]["stable"]["version"])' \
                "${cpp_ci}/.github/actions/toolchain/toolchains.json")" || return 1
        "${ci_local[@]}" --list | awk -v stable="${stable}" -v checks="${own_checks[*]}" -v builds="${own_builds[*]}" '
                BEGIN { split(stable, s, " ") }
                /^run, in order:/ { listing = 1; next }
                listing && NF == 0 { exit }
                listing {
                        leg = $1
                        on_stable = leg ~ ("-(" s[1] "|" s[2] ")$")
                        if (!woven && seen_stable && !on_stable) { print builds; woven = 1 }
                        seen_stable = seen_stable || on_stable
                        print leg
                        if (leg == "format") print checks
                }
                END { if (!woven) print builds }' | tr ' ' '\n' | sed '/^$/d'
}

run_leg() {
        case "$1" in
                layering) layering > "${out}/$1.log" 2>&1 ;;
                site) site > "${out}/$1.log" 2>&1 ;;
                codegen) codegen > "${out}/$1.log" 2>&1 ;;
                std-test-suite) std_test_suite > "${out}/$1.log" 2>&1 ;;
                *) "${ci_local[@]}" "$1" > "${out}/$1.run" 2>&1 ;;
        esac
}

keep_going=0
legs=()
for argument in "$@"; do
        case "${argument}" in
                --list) all_legs; exit ;;
                --quick) legs+=("${quick_legs[@]}") ;;
                -k|--keep-going) keep_going=1 ;;
                *) legs+=("${argument}") ;;
        esac
done
mapfile -t known < <(all_legs) || exit 1
[ "${#legs[@]}" -gt 0 ] || legs=("${known[@]}")
for leg in "${legs[@]}"; do
        printf '%s\n' "${known[@]}" | grep -qx -- "${leg}" \
                || { echo "no leg named ${leg}; tools/ci-local.sh --list names them" >&2; exit 2; }
done

mkdir -p "${out}"
results=()
failed=0
for leg in "${legs[@]}"; do
        if [ "${failed}" -eq 1 ] && [ "${keep_going}" -eq 0 ]; then
                results+=("$(printf '%-4s %-26s' skip "${leg}")")
                continue
        fi
        start="$(date +%s)"
        echo "== ${leg}"
        if run_leg "${leg}"; then
                verdict="pass"
        else
                verdict="FAIL"
                failed=1
        fi
        results+=("$(printf '%-4s %-26s %5ss  %s' "${verdict}" "${leg}" "$(( $(date +%s) - start ))" "${out}/${leg}.log")")
done
printf '%s\n' "${results[@]}"
exit "${failed}"
