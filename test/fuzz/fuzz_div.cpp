//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint.hpp>               // bit_int
#include <xstd/ints/cstdlib/div.hpp>           // div
#include <xstd/ints/cstdlib/div_ceil.hpp>      // div_ceil
#include <xstd/ints/cstdlib/div_euclid.hpp>    // div_euclid
#include <xstd/ints/cstdlib/div_floor.hpp>     // div_floor
#include <xstd/ints/cstdlib/div_result.hpp>    // div_result
#include <xstd/ints/limits.hpp>                // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp> // is_signed_v
#include <fuzz/input.hpp>                      // check, integer_types, operand
#include <cstddef>                             // size_t
#include <cstdint>                             // uint8_t
#include <span>                                // span
#include <tuple>                               // apply

namespace {

// Wide enough that every operand, quotient and product of the fuzzed types is exact, MIN / -1 included.
using wide = xstd::bit_int<256>;

enum class rounding
{
        toward_zero,
        floor,
        ceil,
        euclid,
};

// The quotient rounded as asked, from the exact truncated one and the sign of what it leaves over.
[[nodiscard]] auto oracle_quotient(wide numer, wide denom, rounding mode)
        -> wide
{
        auto const qT      = static_cast<wide>(numer / denom);
        auto const rT      = static_cast<wide>(numer - denom * qT);
        auto const against = rT != 0 and (rT < 0) != (denom < 0);
        auto const along   = rT != 0 and (rT < 0) == (denom < 0);
        switch (mode) {
                case rounding::toward_zero:
                        break;
                case rounding::floor:
                        return against ? static_cast<wide>(qT - 1) : qT;
                case rounding::ceil:
                        return along ? static_cast<wide>(qT + 1) : qT;
                case rounding::euclid:
                        return rT < 0 ? (denom > 0 ? static_cast<wide>(qT - 1) : static_cast<wide>(qT + 1)) : qT;
        }
        return qT;
}

// The quotient fits T exactly; the remainder is numer - denom * quotient, which an unsigned ceiling wraps.
template<class T>
[[nodiscard]] auto agrees(xstd::div_result<T> result, T numer, T denom, rounding mode)
        -> bool
{
        auto const q = oracle_quotient(static_cast<wide>(numer), static_cast<wide>(denom), mode);
        auto const r = static_cast<wide>(static_cast<wide>(numer) - static_cast<wide>(denom) * q);
        return static_cast<wide>(result.quotient) == q and result.remainder == static_cast<T>(r);
}

template<class T>
auto check_at(std::span<std::uint8_t const> input)
        -> void
{
        // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
        auto const& numer = fuzz::operand<T>(input, 0UZ);
        auto const& denom = fuzz::operand<T>(input, 1UZ);
        // The preconditions every rounding shares: a nonzero denominator, and a quotient that fits.
        if (denom == T{0}) {
                return;
        }
        if constexpr (xstd::is_signed_v<T>) {
                if (numer == xstd::numeric_limits<T>::min() and denom == T{-1}) {
                        return;
                }
        }
        fuzz::check(agrees(xstd::div(numer, denom), numer, denom, rounding::toward_zero));
        fuzz::check(agrees(xstd::div_floor(numer, denom), numer, denom, rounding::floor));
        fuzz::check(agrees(xstd::div_ceil(numer, denom), numer, denom, rounding::ceil));
        fuzz::check(agrees(xstd::div_euclid(numer, denom), numer, denom, rounding::euclid));
}

} // namespace

extern "C" auto LLVMFuzzerTestOneInput(std::uint8_t const* data, std::size_t size)
        -> int
{
        auto const input = std::span(data, size);
        std::apply([&]<class... T>(T...) -> void { (check_at<T>(input), ...); }, fuzz::integer_types{});
        return 0;
}
