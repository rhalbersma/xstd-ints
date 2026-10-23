//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_BENCH_MACHINE_HPP
#define XSTD_INTS_BENCH_MACHINE_HPP

#include <nanobench.h> // Bench, doNotOptimizeAway, Result
#include <array>       // array
#include <chrono>      // milliseconds
#include <cstdint>     // uint64_t
#include <fstream>     // ifstream
#include <string>      // getline, stoul, string, to_string
#include <utility>     // pair

#ifdef __APPLE__
#include <sys/sysctl.h> // sysctlbyname
#endif

// What a results table names its machine and compiler by, read from the machine itself.
namespace bench {

// The core clock, as the fastest rate of a chain of dependent adds: one cycle each on every core this runs on.
[[nodiscard]] inline auto clock_ghz()
        -> double
{
#if defined(__GNUC__) and (defined(__x86_64__) or defined(__aarch64__))
        constexpr auto adds = 4096U;
        auto bench          = ankerl::nanobench::Bench().output(nullptr).minEpochTime(std::chrono::milliseconds{10}).epochs(15);
        bench.run([] -> void {
                auto sum       = std::uint64_t{0U};
                auto const one = std::uint64_t{1U};
#pragma GCC unroll 16
                for (auto i = 0U; i < adds; ++i) {
#ifdef __x86_64__
                        asm volatile("add %1, %0" : "+r"(sum) : "r"(one));
#else
                        asm volatile("add %0, %0, %1" : "+r"(sum) : "r"(one));
#endif
                }
                ankerl::nanobench::doNotOptimizeAway(sum);
        });
        return adds * 1e-9 / bench.results().back().minimum(ankerl::nanobench::Result::Measure::elapsed);
#else
        return 0.0;
#endif
}

// The value of the first line of /proc/cpuinfo that starts with `key`, or nothing.
[[nodiscard]] inline auto cpuinfo(std::string const& key)
        -> std::string
{
        auto file = std::ifstream{"/proc/cpuinfo"};
        for (auto line = std::string{}; std::getline(file, line);) {
                if (line.starts_with(key)) {
                        auto const colon = line.find(':');
                        return colon == std::string::npos ? std::string{} : line.substr(line.find_first_not_of(' ', colon + 1U));
                }
        }
        return {};
}

// The brand string where the system has one; on Arm Linux the core named by its part number.
[[nodiscard]] inline auto cpu_name()
        -> std::string
{
#ifdef __APPLE__
        auto brand = std::array<char, 256>{};
        auto size  = brand.size();
        if (sysctlbyname("machdep.cpu.brand_string", brand.data(), &size, nullptr, 0) == 0) {
                return brand.data();
        }
#endif
        // A hypervisor may hide the generation behind a generic brand, so the numbers that identify it come along.
        if (auto model = cpuinfo("model name"); not model.empty()) {
                return model + " (family " + cpuinfo("cpu family") + ", model " + cpuinfo("model\t") + ", stepping " + cpuinfo("stepping") + ")";
        }
        // Arm's own parts, as the Technical Reference Manual of each core gives its MIDR part number.
        constexpr auto arm_parts = std::array<std::pair<unsigned long, char const*>, 6>{{
                {0xd0cUL, "Arm Neoverse N1"},
                {0xd40UL, "Arm Neoverse V1"},
                {0xd49UL, "Arm Neoverse N2"},
                {0xd4fUL, "Arm Neoverse V2"},
                {0xd84UL, "Arm Neoverse V3"},
                {0xd8eUL, "Arm Neoverse N3"},
        }};
        auto const implementer   = cpuinfo("CPU implementer");
        auto const part          = cpuinfo("CPU part");
        if (implementer == "0x41" and not part.empty()) {
                for (auto const& [number, name] : arm_parts) {
                        if (std::stoul(part, nullptr, 16) == number) {
                                return name;
                        }
                }
        }
        return part.empty() ? std::string{"an unidentified CPU"} : "implementer " + implementer + ", part " + part;
}

[[nodiscard]] inline auto compiler_name()
        -> std::string
{
#if defined(__clang__)
        return "Clang " __clang_version__;
#elif defined(__GNUC__)
        return "GCC " __VERSION__;
#elif defined(_MSC_VER)
        return "MSVC " + std::to_string(_MSC_FULL_VER);
#else
        return "an unidentified compiler";
#endif
}

} // namespace bench

#endif // XSTD_INTS_BENCH_MACHINE_HPP
