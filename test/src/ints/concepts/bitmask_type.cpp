//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/bitmask_type.hpp>      // bitmask_type
#include <xstd/ints/concepts/bitwise_operators.hpp> // bitwise_operators
#include <test/exact_width_types.hpp>               // exact_width_signed_integer_types, exact_width_unsigned_integer_types
#include <boost/test/unit_test.hpp>                 // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <bitset>                                   // bitset
#include <charconv>                                 // chars_format
#include <concepts>                                 // signed_integral
#include <filesystem>                               // copy_options, directory_options, perm_options, perms
#include <future>                                   // launch
#include <ios>                                      // ios_base
#include <regex>                                    // regex_constants

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(BitmaskType)

BOOST_AUTO_TEST_CASE_TEMPLATE(AdmitsTheUnsignedExactWidthTypesThroughCv, T, test::exact_width_unsigned_integer_types)
{
        static_assert(xstd::bitmask_type<T>);
        static_assert(xstd::bitmask_type<T const>);
        static_assert(xstd::bitmask_type<T volatile>);
        static_assert(xstd::bitmask_type<T const volatile>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(RejectsTheSignedExactWidthTypesThroughCv, T, test::exact_width_signed_integer_types)
{
        static_assert(not xstd::bitmask_type<T>);
        static_assert(not xstd::bitmask_type<T const>);
        BOOST_CHECK(true);
}

// [bitmask.types] allows any integer type; this admits the unsigned ones, the narrow ones through promotion.
BOOST_AUTO_TEST_CASE(TheIntegerMembersAreTheUnsignedIntegers)
{
        static_assert(xstd::bitmask_type<unsigned char>);
        static_assert(xstd::bitmask_type<unsigned short>);
        static_assert(xstd::bitmask_type<unsigned int>);
        static_assert(xstd::bitmask_type<unsigned long long>);

        static_assert(not xstd::bitmask_type<signed char>);
        static_assert(not xstd::bitmask_type<int>);
        static_assert(not xstd::bitmask_type<long long>);
        static_assert(not xstd::bitmask_type<bool>);
        static_assert(not xstd::bitmask_type<char>);
        static_assert(not xstd::bitmask_type<char32_t>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(StdBitsetIsOne)
{
        static_assert(xstd::bitmask_type<std::bitset<8>>);
        static_assert(xstd::bitmask_type<std::bitset<100> const>);
        BOOST_CHECK(true);
}

// The standard's own bitmask types, each an enumeration overloading the operators in every library tested.
BOOST_AUTO_TEST_CASE(TheStandardBitmaskTypesAreAdmitted)
{
        static_assert(xstd::bitmask_type<std::filesystem::perms>);
        static_assert(xstd::bitmask_type<std::filesystem::perm_options>);
        static_assert(xstd::bitmask_type<std::filesystem::copy_options>);
        static_assert(xstd::bitmask_type<std::filesystem::directory_options>);
        static_assert(xstd::bitmask_type<std::launch>);
        static_assert(xstd::bitmask_type<std::chars_format>);
        static_assert(xstd::bitmask_type<std::regex_constants::syntax_option_type>);
        static_assert(xstd::bitmask_type<std::regex_constants::match_flag_type>);

        // An implementation may make these a signed int, which the unsigned narrowing then turns away.
        static_assert(xstd::bitmask_type<std::ios_base::fmtflags> or std::signed_integral<std::ios_base::fmtflags>);
        static_assert(xstd::bitmask_type<std::ios_base::iostate> or std::signed_integral<std::ios_base::iostate>);
        static_assert(xstd::bitmask_type<std::ios_base::openmode> or std::signed_integral<std::ios_base::openmode>);
        BOOST_CHECK(true);
}

namespace {

enum class plain_flags : unsigned {
        none = 0,
        a    = 1,
};

enum class flags : unsigned {
        none = 0,
        a    = 1,
        b    = 2,
};

[[nodiscard]] constexpr auto operator~(flags x) noexcept
        -> flags
{
        return static_cast<flags>(~static_cast<unsigned>(x));
}

[[nodiscard]] constexpr auto operator&(flags x, flags y) noexcept
        -> flags
{
        return static_cast<flags>(static_cast<unsigned>(x) & static_cast<unsigned>(y));
}

[[nodiscard]] constexpr auto operator^(flags x, flags y) noexcept
        -> flags
{
        return static_cast<flags>(static_cast<unsigned>(x) ^ static_cast<unsigned>(y));
}

[[nodiscard]] constexpr auto operator|(flags x, flags y) noexcept
        -> flags
{
        return static_cast<flags>(static_cast<unsigned>(x) | static_cast<unsigned>(y));
}

constexpr auto operator&=(flags& x, flags y) noexcept
        -> flags&
{
        return x = x & y;
}

constexpr auto operator^=(flags& x, flags y) noexcept
        -> flags&
{
        return x = x ^ y;
}

constexpr auto operator|=(flags& x, flags y) noexcept
        -> flags&
{
        return x = x | y;
}

} // namespace

// An enumeration is one by its operators alone: with them it is admitted, without them it is not.
BOOST_AUTO_TEST_CASE(AnEnumerationIsOneByItsOperators)
{
        static_assert(xstd::bitmask_type<flags>);
        static_assert(not xstd::bitmask_type<plain_flags>);
        static_assert(((flags::a ^ flags::b) & flags::b) == flags::b);
        static_assert([] -> flags {
                auto x = flags::a;
                x |= flags::b;
                x &= ~flags::a;
                x ^= flags::a;
                return x;
        }() == (flags::a | flags::b));
        static_assert(not xstd::bitwise_operators<flags>);
        BOOST_CHECK(true);
}

namespace {

template<xstd::bitmask_type T>
[[nodiscard]] constexpr auto most_refined(T) noexcept
        -> int
{
        return 1;
}

template<xstd::bitwise_operators T>
[[nodiscard]] constexpr auto most_refined(T) noexcept
        -> int
{
        return 2;
}

} // namespace

// bitwise_operators refines bitmask_type, so where both hold the overload on it is chosen as the more constrained.
BOOST_AUTO_TEST_CASE(BitwiseOperatorsRefinesIt)
{
        static_assert(most_refined(0U) == 2);
        static_assert(most_refined(std::bitset<8>()) == 2);
        static_assert(most_refined(flags::a) == 1);
        static_assert(most_refined(std::filesystem::perms::none) == 1);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
