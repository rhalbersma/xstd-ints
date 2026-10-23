#!/usr/bin/env bash
#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Runs the GitHub Actions jobs' Linux legs on what setup-toolchain.sh installs, so a change fails here, fast, before
# it waits on CI's runners. Legs run cheapest first and the run stops at the first that fails. The site leg builds the
# documentation as the Pages workflow does. Each leg works under build/ci-local/<leg> and logs to
# build/ci-local/<leg>.log, and the run ends in one line per leg.
#
#   tools/ci-local.sh --quick             before a commit: format, actionlint, GCC 15, clang 22 and clang-tidy 22
#   tools/ci-local.sh                     before a push or a pull request: every leg
#   tools/ci-local.sh gcc-17 tidy-24      the legs named, in the order given
#   tools/ci-local.sh --keep-going ...    every leg asked for, whatever fails
#   tools/ci-local.sh --list              the legs, in the order a full run takes them
#
# CI_LOCAL_BUILD_TYPE picks the suite legs' build type, Debug by default; CI runs both Debug and Release.
#
# Not here, having no Linux equivalent: MSVC, clang-cl, MinGW and Apple Clang. Emscripten, CodeQL, and the sanitizer
# ladder's other rungs are CI's too.

set -uo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
readonly root
readonly out="${root}/build/ci-local"
readonly build_type="${CI_LOCAL_BUILD_TYPE:-Debug}"
readonly jobs="$(nproc)"
readonly local_bin="${HOME:-/root}/.local/bin"
cmake="${local_bin}/cmake"
[ -x "${cmake}" ] || cmake="cmake"
readonly cmake

# Cheapest first, so a slip in formatting or a workflow fails in seconds rather than after an hour of compiling.
readonly all_legs=(format actionlint site gcc-15 clang-22 tidy-22 codegen std-test-suite consumption gcc-16 clang-23 gcc-17 clang-24
                   libcxx-22 i686 gcc-bmi2 clang-bmi2 tidy-23 tidy-24 coverage asan-gcc ubsan-gcc ubsan-clang
                   implicit-conversion)
readonly quick_legs=(format actionlint gcc-15 clang-22 tidy-22)

# Each clang uses the libstdc++ of its own rung, as on the runners, rather than the newest GCC installed.
gcc_for_clang() {
        case "$1" in
                22) echo "--gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/15" ;;
                23) echo "--gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/16" ;;
                24) echo "--gcc-toolchain=/opt/gcc-latest" ;;
        esac
}

# Configure, build and test the suite: <leg> <compiler> <flags> <build type> [CMake arguments...]
suite() {
        local leg="$1" cxx="$2" flags="$3" type="$4"
        shift 4
        local dir="${out}/${leg}"
        "${cmake}" -S "${root}" -B "${dir}" -G Ninja -DCMAKE_BUILD_TYPE="${type}" -DCMAKE_CXX_COMPILER="${cxx}" \
                -DCMAKE_CXX_FLAGS="${flags}" "$@" \
                && "${cmake}" --build "${dir}" --parallel "${jobs}" \
                && ctest --test-dir "${dir}" --output-on-failure --parallel "${jobs}"
}

# A suite leg on another standard library, a trunk one or another target links a Boost.Test built for it:
# <leg> <compiler> [flags...]. A failed build fails the leg, rather than leave it on the installed Boost.Test.
boost_test_for() {
        local leg="$1"
        shift
        "${root}/tools/boost-test.sh" "${out}/boost-test-${leg}" "$@"
}

# The suite on a Boost.Test of its own: <leg> <compiler> <flags> <build type> [CMake arguments...]
suite_with_boost_test() {
        local leg="$1" cxx="$2" flags="$3" type="$4" package
        shift 4
        # shellcheck disable=SC2086
        package="$(boost_test_for "${leg}" "${cxx}" ${flags})" || return 1
        suite "${leg}" "${cxx}" "${flags}" "${type}" "$@" "${package}"
}

# The sanitizer ladder's legs, on the stable rung, with its flags and options. The ignorelist keeps implicit-conversion
# to the code under test, as the job's does; Boost sits in /usr/local here where vcpkg_installed holds it there.
sanitizer() {
        local leg="$1" cxx="$2" flags="$3" options="$4"
        local ignorelist="${out}/sanitizer-ignorelist.txt"
        if [ "${leg}" = implicit-conversion ]; then
                printf 'src:%s\n' '*/include/c++/*' '*/lib/clang/*/include/*' '*/usr/local/include/*' '*/usr/include/*' > "${ignorelist}"
                flags="${flags} -fsanitize-ignorelist=${ignorelist}"
        fi
        (
                export "${options?}"
                suite "${leg}" "${cxx}" "${flags}" Debug -DCMAKE_EXE_LINKER_FLAGS="${flags}"
        )
}

# cpp-ci's script for a job, at the commit .github/workflows/clang-tidy.yml pins, fetched once into the leg's tree.
cpp_ci_script() {
        local name="$1" sha script
        sha="$(grep -oE 'rhalbersma/cpp-ci/\.github/workflows/clang-tidy\.yml@[0-9a-f]{40}' "${root}/.github/workflows/clang-tidy.yml" | head -1)"
        sha="${sha##*@}"
        [ -n "${sha}" ] || { echo "no cpp-ci pin in .github/workflows/clang-tidy.yml" >&2; return 1; }
        script="${out}/cpp-ci/${sha}/${name}.sh"
        if [ ! -s "${script}" ]; then
                mkdir -p "${script%/*}"
                if ! curl -fsSL --retry 3 -o "${script}.part" \
                        "https://raw.githubusercontent.com/rhalbersma/cpp-ci/${sha}/.github/actions/${name}/${name}.sh"; then
                        rm -f "${script}.part"
                        return 1
                fi
                mv "${script}.part" "${script}"
        fi
        echo "${script}"
}

# The clang-tidy job: its configure, then cpp-ci's clang-tidy script with the inputs the job's stub passes it.
tidy() {
        local version="$1" dir="${out}/tidy-$1" script
        script="$(cpp_ci_script clang-tidy)" || return 1
        "${cmake}" -S "${root}" -B "${dir}" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER="clang++-${version}" \
                -DCMAKE_CXX_FLAGS="$(gcc_for_clang "${version}")" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                -DCMAKE_CXX_SCAN_FOR_MODULES=OFF || return 1
        (cd "${root}" && CLANG_TIDY_VERSION="${version}" BUILD_DIR="${dir}" TEST_DIR=test \
                SELF_SUFFICIENCY_REGEX="${dir}/test/header_self_sufficiency/.*" INCLUDE_EXCLUDE='include/xstd/config/*' \
                ADAPTED_DIRS="${dir}/_deps/absl_int128-src ${dir}/_deps/boost_int128-src/include" \
                REPORT="${out}/tidy-${version}-report.txt" bash "${script}")
}

# The coverage job: the suite instrumented on the stable GCC, then cpp-ci's gcovr script with the job's thresholds.
coverage() {
        local dir="${out}/coverage" script
        script="$(cpp_ci_script gcovr)" || return 1
        "${cmake}" -S "${root}" -B "${dir}" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++-15 \
                -DCMAKE_CXX_FLAGS="--coverage -O0" -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
                && "${cmake}" --build "${dir}" --parallel "${jobs}" \
                && ctest --test-dir "${dir}" --output-on-failure --parallel "${jobs}" \
                && (cd "${root}" && PATH="${local_bin}:${PATH}" GCOV=gcov-15 BUILD_DIR="${dir}" \
                        EXCLUDE="test/.* build/.*" FAIL_UNDER_LINE=100 FAIL_UNDER_BRANCH=100 SEARCH_PATHS="${dir}" \
                        bash "${script}")
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

# The std-test-suite job: libc++'s and libstdc++'s tests of what xstd mirrors, rewritten onto it, each on its own library.
std_test_suite() {
        local dir="${out}/std-test-suite"
        "${cmake}" -S "${root}" -B "${dir}/libcxx" -G Ninja -DCMAKE_CXX_COMPILER=clang++-22 -DCMAKE_CXX_FLAGS=-stdlib=libc++ \
                -DCMAKE_EXE_LINKER_FLAGS=-stdlib=libc++ -DBUILD_TESTING=OFF -DXSTD_INTS_STD_TEST_SUITES=ON \
                && "${cmake}" --build "${dir}/libcxx" --parallel "${jobs}" \
                && ctest --test-dir "${dir}/libcxx" --output-on-failure --parallel "${jobs}" \
                && "${cmake}" -S "${root}" -B "${dir}/libstdcxx" -G Ninja -DCMAKE_CXX_COMPILER=g++-15 \
                        -DBUILD_TESTING=OFF -DXSTD_INTS_STD_TEST_SUITES=ON \
                && "${cmake}" --build "${dir}/libstdcxx" --parallel "${jobs}" \
                && ctest --test-dir "${dir}/libstdcxx" --output-on-failure --parallel "${jobs}"
}

# The consumption job: install the library, then build and run each consumer model under test/consumer.
consumption() {
        local dir="${out}/consumption" model
        rm -rf "${dir}"
        "${cmake}" -S "${root}" -B "${dir}/library" -G Ninja -DCMAKE_CXX_COMPILER=g++-15 -DBUILD_TESTING=OFF \
                && "${cmake}" --install "${dir}/library" --prefix "${dir}/install-prefix" || return 1
        for model in find_package add_subdirectory fetch_content; do
                [ -f "${root}/test/consumer/${model}/CMakeLists.txt" ] || continue
                "${cmake}" -S "${root}/test/consumer/${model}" -B "${dir}/${model}" -G Ninja -DCMAKE_CXX_COMPILER=g++-15 \
                        -DCMAKE_PREFIX_PATH="${dir}/install-prefix" \
                        && "${cmake}" --build "${dir}/${model}" \
                        && "${dir}/${model}/main" || return 1
        done
}

# The actionlint job, at its pinned release, over every workflow.
actionlint() {
        "${local_bin}/actionlint" -shellcheck= "${root}"/.github/workflows/*.yml
}

# The format gate: the stable rung's clang-format over include/ and test/, reporting rather than rewriting.
format() {
        find "${root}/include" "${root}/test" -regex '.*\.\(hpp\|cpp\)' -print0 | xargs -0 clang-format-22 --dry-run --Werror
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

run_leg() {
        case "$1" in
                format) format ;;
                actionlint) actionlint ;;
                site) site ;;
                gcc-1[56]) suite "$1" "g++-${1#gcc-}" "" "${build_type}" ;;
                clang-2[23]) suite "$1" "clang++-${1#clang-}" "$(gcc_for_clang "${1#clang-}")" "${build_type}" ;;
                # The trunk libstdc++ is newer than the GCC 15 that built the installed Boost.Test.
                gcc-17) suite_with_boost_test "$1" g++-17 "" "${build_type}" ;;
                clang-24) suite_with_boost_test "$1" clang++-24 "$(gcc_for_clang 24)" "${build_type}" ;;
                libcxx-22) suite_with_boost_test "$1" clang++-22 "-stdlib=libc++" "${build_type}" -DCMAKE_EXE_LINKER_FLAGS=-stdlib=libc++ ;;
                # One rung in Debug, as the i686 job runs it.
                i686) suite_with_boost_test "$1" g++-15 "-m32" Debug ;;
                # One rung in Release, as the BMI2 jobs run them.
                gcc-bmi2) suite "$1" g++-15 "-march=x86-64-v3" Release ;;
                clang-bmi2) suite "$1" clang++-22 "$(gcc_for_clang 22) -march=x86-64-v3" Release ;;
                tidy-2[234]) tidy "${1#tidy-}" ;;
                codegen) codegen ;;
                std-test-suite) std_test_suite ;;
                consumption) consumption ;;
                coverage) coverage ;;
                asan-gcc) sanitizer "$1" g++-15 "-fsanitize=address -fno-omit-frame-pointer" ASAN_OPTIONS=detect_leaks=1 ;;
                ubsan-gcc) sanitizer "$1" g++-15 "-fsanitize=undefined -fno-sanitize-recover=undefined" UBSAN_OPTIONS=print_stacktrace=1 ;;
                ubsan-clang)
                        sanitizer "$1" clang++-22 "$(gcc_for_clang 22) -fsanitize=undefined -fno-sanitize-recover=undefined" \
                                UBSAN_OPTIONS=print_stacktrace=1 ;;
                implicit-conversion)
                        sanitizer "$1" clang++-22 "$(gcc_for_clang 22) -fsanitize=implicit-conversion -fno-sanitize-recover=implicit-conversion" \
                                UBSAN_OPTIONS=print_stacktrace=1 ;;
                *) echo "no leg named $1; tools/ci-local.sh --list names them" >&2; return 2 ;;
        esac
}

keep_going=0
legs=()
for argument in "$@"; do
        case "${argument}" in
                --list) printf '%s\n' "${all_legs[@]}"; exit 0 ;;
                --quick) legs+=("${quick_legs[@]}") ;;
                -k|--keep-going) keep_going=1 ;;
                *) legs+=("${argument}") ;;
        esac
done
[ "${#legs[@]}" -gt 0 ] || legs=("${all_legs[@]}")

mkdir -p "${out}"
results=()
failed=0
for leg in "${legs[@]}"; do
        if [ "${failed}" -eq 1 ] && [ "${keep_going}" -eq 0 ]; then
                results+=("$(printf '%-4s %-19s' skip "${leg}")")
                continue
        fi
        start="$(date +%s)"
        echo "== ${leg}"
        if run_leg "${leg}" > "${out}/${leg}.log" 2>&1; then
                verdict="pass"
        else
                verdict="FAIL"
                failed=1
        fi
        results+=("$(printf '%-4s %-19s %5ss  %s' "${verdict}" "${leg}" "$(( $(date +%s) - start ))" "${out}/${leg}.log")")
done
printf '%s\n' "${results[@]}"
exit "${failed}"
