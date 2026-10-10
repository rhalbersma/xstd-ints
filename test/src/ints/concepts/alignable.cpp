//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/alignable.hpp>        // alignable, nothrow_alignable
#include <xstd/ints/concepts/signed_integer.hpp>   // signed_integer
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdint/int128.hpp>            // int128, uint128
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <compare>                                 // the result of <=> over built-ins
#include <concepts>                                // constructible_from
#include <cstddef>                                 // ptrdiff_t, size_t
#include <cstdint>                                 // exact-width integer types, uintptr_t
#include <type_traits>                             // is_nothrow_..._v
#include <utility>                                 // declval

#if __has_include(<absl/numeric/int128.h>)
#define TEST_HAS_ABSL_INT128
#include <absl/numeric/int128.h> // uint128
#endif

namespace {

// Every unsigned_integer is alignable, no signed_integer is, and alignable carries the way back to size_t.
template<class T>
constexpr auto refines_alignable =
        (not xstd::unsigned_integer<T> or xstd::alignable<T>) and
        (not xstd::signed_integer<T> or not xstd::alignable<T>) and
        (not xstd::alignable<T> or std::constructible_from<std::size_t, T>);

} // namespace

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(Alignable)

BOOST_AUTO_TEST_CASE(AdmitsTheUnsignedIntegers)
{
        static_assert(xstd::alignable<unsigned char>);
        static_assert(xstd::alignable<unsigned short>);
        static_assert(xstd::alignable<unsigned>);
        static_assert(xstd::alignable<unsigned long>);
        static_assert(xstd::alignable<unsigned long long>);
        static_assert(xstd::alignable<std::size_t>);
        static_assert(xstd::alignable<std::uintptr_t>);
        static_assert(xstd::alignable<std::uint8_t>);
        static_assert(xstd::alignable<std::uint64_t>);
        static_assert(xstd::alignable<xstd::uint128>);
#ifdef __BITINT_MAXWIDTH__
        static_assert(xstd::alignable<unsigned _BitInt(8)>);
        static_assert(xstd::alignable<unsigned _BitInt(24)>);
        static_assert(xstd::alignable<unsigned _BitInt(64)>);
#endif
        BOOST_CHECK(true);
}

// Nothing is excluded by name: each of these fails a clause it cannot satisfy.
BOOST_AUTO_TEST_CASE(AndRejectsTheRestOnTheirMerits)
{
        // Signed, so a wrapped sum would be undefined rather than a value the precondition can ask about.
        static_assert(not xstd::alignable<signed char>);
        static_assert(not xstd::alignable<short>);
        static_assert(not xstd::alignable<int>);
        static_assert(not xstd::alignable<long long>);
        static_assert(not xstd::alignable<std::ptrdiff_t>);
        static_assert(not xstd::alignable<xstd::int128>);
#ifdef __BITINT_MAXWIDTH__
        static_assert(not xstd::alignable<signed _BitInt(24)>);
#endif
        // One bit wide, which is what the digits clause is for: bool's one plus one is still one.
        static_assert(not xstd::alignable<bool>);
        static_assert(xstd::numeric_limits<bool>::digits == 1);
        // What gets aligned, yet no integer: numeric_limits is not specialized for it, which is not_pointer for free.
        static_assert(not xstd::alignable<void*>);
        static_assert(not xstd::alignable<std::uint8_t*>);
        BOOST_CHECK(true);
}

// The point of the concept: it is integer_class's opening clauses and then it stops.
BOOST_AUTO_TEST_CASE(IsIntegerLight)
{
        static_assert(refines_alignable<unsigned char>);
        static_assert(refines_alignable<unsigned>);
        static_assert(refines_alignable<unsigned long long>);
        static_assert(refines_alignable<std::size_t>);
        static_assert(refines_alignable<std::uint8_t>);
        static_assert(refines_alignable<signed char>);
        static_assert(refines_alignable<int>);
        static_assert(refines_alignable<bool>);
        static_assert(refines_alignable<xstd::uint128>);
        static_assert(refines_alignable<xstd::int128>);
#ifdef __BITINT_MAXWIDTH__
        static_assert(refines_alignable<unsigned _BitInt(24)>);
        static_assert(refines_alignable<signed _BitInt(24)>);
#endif

        static_assert(refines_alignable<char8_t>);
        static_assert(refines_alignable<char16_t>);
        static_assert(refines_alignable<char32_t>);

        // And the converse fails, which is what makes it light rather than a synonym for unsigned_integer.
        static_assert(xstd::alignable<char8_t>);
        static_assert(xstd::alignable<char16_t>);
        static_assert(xstd::alignable<char32_t>);
        static_assert(not xstd::unsigned_integer<char8_t>);
        static_assert(not xstd::unsigned_integer<char16_t>);
        static_assert(not xstd::unsigned_integer<char32_t>);
        BOOST_CHECK(true);
}

// Cv-transparent, as integer_class is: without stripping, the answer would turn on the 128-bit type being a class.
BOOST_AUTO_TEST_CASE(IsCvTransparent)
{
        static_assert(xstd::alignable<unsigned const>);
        static_assert(xstd::alignable<unsigned volatile>);
        static_assert(xstd::alignable<unsigned const volatile>);
        static_assert(xstd::alignable<std::size_t const volatile>);
        static_assert(xstd::alignable<xstd::uint128 const volatile>);
        static_assert(xstd::alignable<char16_t const volatile>);

        // And a qualifier turns no answer into a yes: the signed half stays out.
        static_assert(not xstd::alignable<int const volatile>);
        static_assert(not xstd::alignable<xstd::int128 const volatile>);
        static_assert(not xstd::alignable<bool const volatile>);

        static_assert(xstd::nothrow_alignable<unsigned const volatile>);
        static_assert(xstd::nothrow_alignable<std::size_t const>);
        BOOST_CHECK(true);
}

// All three pass and return a T without naming a special member, and the nothrow refinement answers for that.
BOOST_AUTO_TEST_CASE(TheNothrowRefinementCoversWhatTheCallSpends)
{
        static_assert(std::is_nothrow_destructible_v<std::size_t>);
        static_assert(std::is_nothrow_copy_constructible_v<xstd::uint128>);

        static_assert(not xstd::nothrow_alignable<xstd::uint128> or std::is_nothrow_move_constructible_v<xstd::uint128>);
        static_assert(not xstd::nothrow_alignable<std::size_t> or std::is_nothrow_destructible_v<std::size_t>);
        BOOST_CHECK(true);
}

// Every comparison /9 gives, not the two the functions reach for: an unasked relation could still throw.
BOOST_AUTO_TEST_CASE(TheNothrowRefinementCoversEveryRelation)
{
        static_assert(noexcept(std::declval<std::size_t const&>() <=> std::declval<std::size_t const&>()));
        static_assert(noexcept(std::declval<std::size_t const&>() == std::declval<std::size_t const&>()));
        static_assert(noexcept(std::declval<std::size_t const&>() != std::declval<std::size_t const&>()));
        static_assert(noexcept(std::declval<std::size_t const&>() < std::declval<std::size_t const&>()));
        static_assert(noexcept(std::declval<std::size_t const&>() > std::declval<std::size_t const&>()));
        static_assert(noexcept(std::declval<std::size_t const&>() <= std::declval<std::size_t const&>()));
        static_assert(noexcept(std::declval<std::size_t const&>() >= std::declval<std::size_t const&>()));

        // And of the types it does hold of, over which all six must be noexcept for the specification to mean anything.
        static_assert(xstd::nothrow_alignable<std::size_t>);
        static_assert(xstd::nothrow_alignable<unsigned>);
        static_assert(xstd::nothrow_alignable<xstd::uint128>);
        BOOST_CHECK(true);
}

// Only visible on a class type without noexcept, which Abseil is: it declares noexcept nowhere.
BOOST_AUTO_TEST_CASE(TheNothrowRefinementNarrowsIt)
{
        static_assert(xstd::nothrow_alignable<unsigned char>);
        static_assert(xstd::nothrow_alignable<std::size_t>);
#ifdef TEST_HAS_ABSL_INT128
        static_assert(xstd::alignable<absl::uint128>);
        static_assert(not xstd::nothrow_alignable<absl::uint128>);
        // The way back out, which is what the address round trip needs of a type wider than a pointer.
        static_assert(std::constructible_from<std::size_t, absl::uint128>);
#endif
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
