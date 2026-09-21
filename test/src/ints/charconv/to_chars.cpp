//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/charconv/to_chars.hpp> // to_chars, to_chars_max_size
#include <xstd/ints/concepts/integer.hpp>  // integer
#include <xstd/ints/cstdint.hpp>           // int128, uint128
#include <test/exact_width_types.hpp>      // std_signed_types, exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <xstd/ints/limits.hpp>            // numeric_limits
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL
#include <array>                           // array
#include <charconv>                        // to_chars, to_chars_result
#include <concepts>                        // same_as
#include <cstddef>                         // size_t
#include <string>                          // string
#include <string_view>                     // string_view
#include <system_error>                    // errc

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CharConv)
BOOST_AUTO_TEST_SUITE(ToChars)

// Named concepts, not bare requires-expressions: a deleted call is non-dependently ill-formed.
template<class T>
concept has_std_to_chars = requires (char* p, T value, int base) {
        { std::to_chars(p, p, value, base) } -> std::same_as<std::to_chars_result>;
};

// Also pins that the constrained overload wins by partial ordering rather than tying.
template<class T>
concept has_xstd_to_chars = requires (char* p, T value) { xstd::to_chars(p, p, value, 10); };

template<class T>
[[nodiscard]] auto rendered(T value, int base)
        -> std::string
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        BOOST_CHECK(result.ec == std::errc{});
        return std::string(buffer.data(), result.ptr);
}

// The standard's, sized generously rather than by to_chars_max_size, to stay honest.
template<class T>
[[nodiscard]] auto rendered_by_std(T value, int base)
        -> std::string
{
        auto buffer = std::array<char, 160>{};
        auto const result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        BOOST_CHECK(result.ec == std::errc{});
        return std::string(buffer.data(), result.ptr);
}

// Which path a 128-bit type takes is the library's and the dialect's, so only agreement is asserted.
BOOST_AUTO_TEST_CASE(DelegatesWhereTheStandardLibraryCovers)
{
        static_assert(has_std_to_chars<int>);
        static_assert(has_std_to_chars<long long>);
        static_assert(has_std_to_chars<unsigned>);

        // bool is rejected explicitly, matching the standard's deleted overload.
        static_assert(not has_std_to_chars<bool>);
        static_assert(not has_xstd_to_chars<bool>);
        static_assert(not has_xstd_to_chars<bool const>);
        static_assert(has_xstd_to_chars<int>);

        // The ambiguous case reaches the digits path too, as libstdc++ produces for __int128.
        struct not_an_integer
        {};

        static_assert(not has_std_to_chars<not_an_integer>);

        static_assert(has_xstd_to_chars<xstd::int128>);
        static_assert(has_xstd_to_chars<xstd::uint128>);
}

BOOST_AUTO_TEST_CASE(StandardIntegralTypesDelegate)
{
        static_assert(not xstd::integer<char>);
        static_assert(has_std_to_chars<char>);
        static_assert(has_std_to_chars<wchar_t>);
        static_assert(has_std_to_chars<char8_t>);
        static_assert(has_std_to_chars<char16_t>);
        static_assert(has_std_to_chars<char32_t>);
        static_assert(has_xstd_to_chars<char>);
        static_assert(has_xstd_to_chars<wchar_t>);
        static_assert(has_xstd_to_chars<char8_t>);
        static_assert(has_xstd_to_chars<char16_t>);
        static_assert(has_xstd_to_chars<char32_t>);
}

// The load-bearing property: the two paths render byte-identically, at every base.
BOOST_AUTO_TEST_CASE_TEMPLATE(DigitsPathMatchesTheStandard, T, test::std_signed_types)
{
        for (auto base = 2; base <= 36; ++base) {
                for (auto const value : {
                             T{0},
                             T{1},
                             T{-1},
                             T{7},
                             T{-7},
                             xstd::numeric_limits<T>::min(),
                             xstd::numeric_limits<T>::max(),
                     }) {
                        BOOST_CHECK_EQUAL(rendered(static_cast<xstd::int128>(value), base), rendered_by_std(value, base));
                }
        }
}

// The worst case is min() in base 2, where a signed type needs two more characters.
BOOST_AUTO_TEST_CASE_TEMPLATE(MaxSizeHoldsTheWorstCase, T, test::exact_width_signed_integer_types)
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const min = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), xstd::numeric_limits<T>::min(), 2);
        BOOST_CHECK(min.ec == std::errc{});
        auto const max = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), xstd::numeric_limits<T>::max(), 2);
        BOOST_CHECK(max.ec == std::errc{});

        // And exactly: one character less does not fit. Per type, as gcov records it.
        auto const short_buffer = xstd::to_chars(buffer.data(), buffer.data() + buffer.size() - 1, xstd::numeric_limits<T>::min(), 2);
        BOOST_CHECK(short_buffer.ec == std::errc::value_too_large);
        BOOST_CHECK(short_buffer.ptr == buffer.data() + buffer.size() - 1);

        // No room at all, answered before any digit is counted: the second return.
        auto const empty = xstd::to_chars(buffer.data(), buffer.data(), xstd::numeric_limits<T>::min(), 2);
        BOOST_CHECK(empty.ec == std::errc::value_too_large);
        BOOST_CHECK(empty.ptr == buffer.data());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(MaxSizeHoldsTheWorstCaseUnsigned, T, test::exact_width_unsigned_integer_types)
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const max = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), xstd::numeric_limits<T>::max(), 2);
        BOOST_CHECK(max.ec == std::errc{});

        auto const short_buffer = xstd::to_chars(buffer.data(), buffer.data() + buffer.size() - 1, xstd::numeric_limits<T>::max(), 2);
        BOOST_CHECK(short_buffer.ec == std::errc::value_too_large);
        BOOST_CHECK(short_buffer.ptr == buffer.data() + buffer.size() - 1);

        // As above: no room at all, answered before any digit is counted.
        auto const empty = xstd::to_chars(buffer.data(), buffer.data(), xstd::numeric_limits<T>::max(), 2);
        BOOST_CHECK(empty.ec == std::errc::value_too_large);
        BOOST_CHECK(empty.ptr == buffer.data());
}

// Ground truth built here rather than borrowed, so a fault cannot produce its own expectation.
BOOST_AUTO_TEST_CASE_TEMPLATE(HexBoundariesMatchGroundTruth, T, test::exact_width_signed_integer_types)
{
        // digits counts value bits, so the boundaries run that many hex characters past the first.
        constexpr auto rest = static_cast<std::size_t>(xstd::numeric_limits<T>::digits - 3) / 4;

        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<T>::max(), 16), "7" + std::string(rest, 'f'));
        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<T>::min(), 16), "-8" + std::string(rest, '0'));

        // And either side of zero, where the sign and the one-digit case meet.
        BOOST_CHECK_EQUAL(rendered(T{-2}, 16), "-2");
        BOOST_CHECK_EQUAL(rendered(T{-1}, 16), "-1");
        BOOST_CHECK_EQUAL(rendered(T{0}, 16), "0");
        BOOST_CHECK_EQUAL(rendered(T{+1}, 16), "1");
        BOOST_CHECK_EQUAL(rendered(T{+2}, 16), "2");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(HexBoundariesMatchGroundTruthUnsigned, T, test::exact_width_unsigned_integer_types)
{
        // No sign bit, so digits is the width and the answer is f's all the way.
        constexpr auto width = static_cast<std::size_t>(xstd::numeric_limits<T>::digits) / 4;

        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<T>::max(), 16), std::string(width, 'f'));
        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<T>::min(), 16), "0");

        BOOST_CHECK_EQUAL(rendered(T{0}, 16), "0");
        BOOST_CHECK_EQUAL(rendered(T{1}, 16), "1");
        BOOST_CHECK_EQUAL(rendered(T{2}, 16), "2");
}

BOOST_AUTO_TEST_CASE(Int128Boundaries)
{
        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<xstd::int128>::min(), 10), "-170141183460469231731687303715884105728");
        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<xstd::int128>::max(), 10), "170141183460469231731687303715884105727");
        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<xstd::uint128>::max(), 10), "340282366920938463463374607431768211455");
        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<xstd::int128>::max(), 16), "7fffffffffffffffffffffffffffffff");
        BOOST_CHECK_EQUAL(rendered(xstd::numeric_limits<xstd::uint128>::max(), 2), std::string(128, '1'));
        BOOST_CHECK_EQUAL(rendered(xstd::int128{0}, 10), "0");
        BOOST_CHECK_EQUAL(rendered(xstd::int128{-1}, 10), "-1");
        BOOST_CHECK_EQUAL(rendered(xstd::int128{35}, 36), "z");

        // The default base, which every call above passes explicitly.
        auto decimal = std::array<char, xstd::to_chars_max_size<xstd::int128>>{};
        auto const wide = xstd::to_chars(decimal.data(), decimal.data() + decimal.size(), xstd::int128{-42});
        BOOST_CHECK_EQUAL(std::string(decimal.data(), wide.ptr), "-42");
        auto const narrow = xstd::to_chars(decimal.data(), decimal.data() + decimal.size(), 42);
        BOOST_CHECK_EQUAL(std::string(decimal.data(), narrow.ptr), "42");

        // The short-buffer return in the unsigned instantiation, counted separately.
        auto buffer = std::array<char, xstd::to_chars_max_size<xstd::uint128>>{};
        auto const truncated = xstd::to_chars(
                buffer.data(), buffer.data(), xstd::numeric_limits<xstd::uint128>::max(), 2
        );
        BOOST_CHECK(truncated.ec == std::errc::value_too_large);
}

// A constant expression, which is what std::formatter<div_result<I>>::format needs for P3391.
template<class T>
[[nodiscard]] consteval auto rendered_at_compile_time(T value, int base, std::string_view expected)
        -> bool
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        return result.ec == std::errc{} and std::string_view{buffer.data(), result.ptr} == expected;
}

BOOST_AUTO_TEST_CASE(UsableInAConstantExpression)
{
        static_assert(rendered_at_compile_time(255, 16, "ff"));
        static_assert(rendered_at_compile_time(-42, 10, "-42"));
        static_assert(rendered_at_compile_time(xstd::numeric_limits<xstd::int128>::min(), 10, "-170141183460469231731687303715884105728"));
        static_assert(rendered_at_compile_time(xstd::numeric_limits<xstd::uint128>::max(), 16, "ffffffffffffffffffffffffffffffff"));
}

// Asked of every type in the lists: a third party is constexpr only where it says so.
template<class T>
[[nodiscard]] consteval auto renders_at_compile_time(T value, int base)
        -> bool
{
        auto buffer = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value, base);
        return result.ec == std::errc{} and result.ptr != buffer.data();
}

BOOST_AUTO_TEST_CASE_TEMPLATE(UsableInAConstantExpressionPerType, T, test::constexpr_exact_width_signed_integer_types)
{
        static_assert(renders_at_compile_time(xstd::numeric_limits<T>::min(), 10));
        static_assert(renders_at_compile_time(xstd::numeric_limits<T>::max(), 16));
        static_assert(renders_at_compile_time(T{0}, 2));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(UsableInAConstantExpressionPerTypeUnsigned, T, test::constexpr_exact_width_unsigned_integer_types)
{
        static_assert(renders_at_compile_time(xstd::numeric_limits<T>::max(), 10));
        static_assert(renders_at_compile_time(xstd::numeric_limits<T>::min(), 36));
}

// A buffer too small reports value_too_large and leaves ptr at last, not a truncation.
BOOST_AUTO_TEST_CASE(ShortBuffer)
{
        auto buffer = std::array<char, 4>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), 123456, 10);
        BOOST_CHECK(result.ec == std::errc::value_too_large);
        BOOST_CHECK(result.ptr == buffer.data() + buffer.size());

        // The same in the signed digits instantiation: ptr at last, not into a partial answer.
        auto const wide = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), xstd::numeric_limits<xstd::int128>::min(), 10);
        BOOST_CHECK(wide.ec == std::errc::value_too_large);
        BOOST_CHECK(wide.ptr == buffer.data() + buffer.size());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
