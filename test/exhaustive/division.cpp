//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib/div.hpp>        // div
#include <xstd/ints/cstdlib/div_ceil.hpp>   // div_ceil
#include <xstd/ints/cstdlib/div_euclid.hpp> // div_euclid
#include <xstd/ints/cstdlib/div_floor.hpp>  // div_floor
#include <sweep.hpp>                        // for_each_row, mix, pack, passed, patterns
#include <array>                            // array
#include <atomic>                           // atomic
#include <cstdint>                          // int16_t, uint16_t, uint64_t
#include <cstdlib>                          // abs
#include <fstream>                          // ifstream
#include <iomanip>                          // hex
#include <iostream>                         // cerr, cout
#include <string>                           // string
#include <type_traits>                      // is_signed_v

namespace {

enum class rounding : std::uint8_t
{
        toward_zero,
        floor,
        euclid,
        ceil
};

constexpr auto roundings = std::array{rounding::toward_zero, rounding::floor, rounding::euclid, rounding::ceil};

constexpr auto names = std::array{"div", "div_floor", "div_euclid", "div_ceil"};

template<class I>
[[nodiscard]] auto divide(rounding how, I n, I d)
        -> xstd::div_result<I>
{
        switch (how) {
                case rounding::toward_zero:
                        return xstd::div(n, d);
                case rounding::floor:
                        return xstd::div_floor(n, d);
                case rounding::euclid:
                        return xstd::div_euclid(n, d);
                case rounding::ceil:
                        return xstd::div_ceil(n, d);
        }
        return {};
}

[[nodiscard]] constexpr auto sign(int x) noexcept
        -> int
{
        return (x > 0) - (x < 0);
}

// The postconditions in int, where every quotient times every denominator fits: they alone determine the result.
template<class I>
[[nodiscard]] auto holds(rounding how, int n, int d, int q, int r)
        -> bool
{
        if constexpr (std::is_signed_v<I>) {
                auto const bounded   = std::abs(r) < std::abs(d);
                auto const signed_as = [&]() -> bool {
                        switch (how) {
                                case rounding::toward_zero:
                                        return r == 0 or sign(r) == sign(n);
                                case rounding::floor:
                                        return r == 0 or sign(r) == sign(d);
                                case rounding::euclid:
                                        return r >= 0;
                                case rounding::ceil:
                                        return r == 0 or sign(r) == -sign(d);
                        }
                        return false;
                }();
                return q * d + r == n and bounded and signed_as;
        } else {
                // The ceiling's remainder is at most zero, which an unsigned type holds modulo 2^16.
                if (how == rounding::ceil) {
                        auto const negated = (exhaustive::patterns - static_cast<unsigned>(r)) % exhaustive::patterns;
                        return (static_cast<unsigned>(q * d + r) % exhaustive::patterns) == static_cast<unsigned>(n) and negated < static_cast<unsigned>(d);
                }
                return q * d + r == n and r < d;
        }
}

using digests = std::array<std::atomic<std::uint64_t>, roundings.size()>;

// Every pair in the domain: no zero denominator, and for a signed type no minimum over minus one.
template<class I>
auto sweep(digests& sums, std::atomic<std::uint64_t>& failures)
        -> void
{
        exhaustive::for_each_row([&](std::uint16_t numer_bits) -> void {
                auto local      = std::array<std::uint64_t, roundings.size()>{};
                auto local_fail = std::uint64_t{0U};
                auto const n    = static_cast<I>(numer_bits);
                for (auto b = 1U; b < exhaustive::patterns; ++b) {
                        auto const d = static_cast<I>(b);
                        if constexpr (std::is_signed_v<I>) {
                                if (n == I{-32768} and d == I{-1}) {
                                        continue;
                                }
                        }
                        for (auto k = 0UZ; k < roundings.size(); ++k) {
                                auto const [q, r] = divide(roundings[k], n, d);
                                local_fail += holds<I>(roundings[k], n, d, q, r) ? 0U : 1U;
                                local[k] += exhaustive::mix(exhaustive::pack(numer_bits, static_cast<std::uint16_t>(b), static_cast<std::uint16_t>(q), static_cast<std::uint16_t>(r)));
                        }
                }
                for (auto k = 0UZ; k < roundings.size(); ++k) {
                        sums[k].fetch_add(local[k], std::memory_order_relaxed);
                }
                failures.fetch_add(local_fail, std::memory_order_relaxed);
        });
}

// Each line of the expected file is a type, a function and a digest in hexadecimal, as the reference wrote them.
[[nodiscard]] auto matches(std::string const& type, digests const& sums, std::string const& path)
        -> bool
{
        auto agree = true;
        for (auto k = 0UZ; k < roundings.size(); ++k) {
                auto in       = std::ifstream(path);
                auto expected = std::uint64_t{0U};
                auto found    = false;
                for (auto t = std::string{}, f = std::string{}; in >> t >> f >> std::hex >> expected >> std::dec;) {
                        if (t == type and f == names[k]) {
                                found = true;
                                break;
                        }
                }
                auto const actual = sums[k].load();
                std::cout << type << ' ' << names[k] << ' ' << std::hex << actual << std::dec << '\n';
                if (not found or actual != expected) {
                        std::cerr << type << ' ' << names[k] << ": " << (found ? "digest differs from the reference" : "no reference digest") << '\n';
                        agree = false;
                }
        }
        return agree;
}

template<class I>
[[nodiscard]] auto check(std::string const& type, std::string const& path)
        -> bool
{
        auto sums     = digests{};
        auto failures = std::atomic<std::uint64_t>{0U};
        sweep<I>(sums, failures);
        auto const postconditions = exhaustive::passed((type + " postconditions").c_str(), failures.load());
        return matches(type, sums, path) and postconditions;
}

} // namespace

// Every 16-bit pair through the four roundings, held to their postconditions and to an independent reference's digests.
auto main(int argc, char* argv[]) -> int
{
        if (argc != 2) {
                std::cerr << "usage: " << argv[0] << " <reference digests>\n";
                return 2;
        }
        auto const path        = std::string(argv[1]);
        auto const is_signed   = check<std::int16_t>("int16", path);
        auto const is_unsigned = check<std::uint16_t>("uint16", path);
        return is_signed and is_unsigned ? 0 : 1;
}
