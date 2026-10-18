//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/rotl.hpp>                  // rotl
#include <xstd/ints/bit/rotr.hpp>                  // rotr
#include <xstd/ints/bit/shl.hpp>                   // shl
#include <xstd/ints/bit/shr.hpp>                   // shr
#include <xstd/ints/limits.hpp>                    // numeric_limits
#include <xstd/ints/type_traits/is_unsigned.hpp>   // is_unsigned_v
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <fuzz/input.hpp>                          // check, integer_types, operand
#include <test/bit/oracle.hpp>                     // oracle_rotl, oracle_shift
#include <cstddef>                                 // size_t
#include <cstdint>                                 // int8_t, uint8_t
#include <span>                                    // span
#include <tuple>                                   // apply, tuple
#include <utility>                                 // cmp_greater, cmp_less

namespace {

// The count types a caller reaches for: int, a size, a 64-bit count, and a narrow one whose extremes are in reach.
using count_types = std::tuple<int, std::size_t, long long, std::int8_t>;

// The count pinned just past the width either way, beyond which every shift gives the same answer.
template<class T, class S>
[[nodiscard]] auto saturated(S s)
        -> long long
{
        constexpr auto limit = static_cast<long long>(xstd::numeric_limits<xstd::make_unsigned_t<T>>::digits) + 1LL;
        return std::cmp_less(s, -limit) ? -limit : std::cmp_greater(s, limit) ? limit
                                                                              : static_cast<long long>(s);
}

template<class T, class S>
auto check_shifts(T x, S s)
        -> void
{
        auto const count = saturated<T>(s);
        fuzz::check(xstd::shl(x, s) == test::oracle_shift(x, count));
        fuzz::check(xstd::shr(x, s) == test::oracle_shift(x, -count));
}

template<class T>
auto check_at(std::span<std::uint8_t const> input)
        -> void
{
        // Bound by reference: Clang 19 crashes emitting a const local _BitInt over 128 bits.
        auto const& x = fuzz::operand<T>(input, 0UZ);
        std::apply([&]<class... S>(S...) -> void { (check_shifts(x, fuzz::operand<S>(input, 1UZ)), ...); }, count_types{});
        if constexpr (xstd::is_unsigned_v<T>) {
                // The rotation count is int at every width, so a negative one turns the other way.
                auto const s = fuzz::operand<int>(input, 1UZ);
                fuzz::check(xstd::rotl(x, s) == test::oracle_rotl(x, s));
                fuzz::check(xstd::rotr(x, s) == test::oracle_rotl(x, -(s % xstd::numeric_limits<T>::digits)));
        }
}

} // namespace

extern "C" auto LLVMFuzzerTestOneInput(std::uint8_t const* data, std::size_t size)
        -> int
{
        auto const input = std::span(data, size);
        std::apply([&]<class... T>(T...) -> void { (check_at<T>(input), ...); }, fuzz::integer_types{});
        return 0;
}
