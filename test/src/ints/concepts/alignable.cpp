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
#include <compare>                                 // strong_ordering
#include <concepts>                                // constructible_from
#include <cstddef>                                 // size_t
#include <cstdint>                                 // exact-width integer types, uintptr_t
#include <limits>                                  // numeric_limits
#include <type_traits>                             // is_nothrow_..._v
#include <utility>                                 // declval

#if __has_include(<absl/numeric/int128.h>)
#define TEST_HAS_ABSL_INT128
#include <absl/numeric/int128.h> // uint128
#endif

namespace {

// Alignable without being an integer: no division, no increments, no compound assignment.
struct light
{
        std::size_t v = 0;

        constexpr light() = default;
        // NOLINTNEXTLINE(misc-explicit-constructor, google-explicit-constructor): implicit on purpose, alignable asking convertible_to<size_t, T>.
        constexpr light(std::size_t n)
            : v(n) {}

        // Explicit is enough the other way, alignable asking only constructible_from<size_t, T> there.
        [[nodiscard]] explicit constexpr operator std::size_t() const
        {
                return v;
        }

        [[nodiscard]] constexpr auto operator<=>(light const&) const -> std::strong_ordering = default;
        [[nodiscard]] constexpr auto operator==(light const&) const -> bool = default;

        [[nodiscard]] constexpr auto operator+(light o) const
                -> light
        {
                return {v + o.v};
        }

        [[nodiscard]] constexpr auto operator-(light o) const
                -> light
        {
                return {v - o.v};
        }

        [[nodiscard]] constexpr auto operator&(light o) const
                -> light
        {
                return {v & o.v};
        }
};

// Every unsigned_integer is alignable, no signed_integer is, and alignable carries the way back to size_t.
template<class T>
constexpr auto refines_alignable =
        (not xstd::unsigned_integer<T> or xstd::alignable<T>) and
        (not xstd::signed_integer<T> or not xstd::alignable<T>) and
        (not xstd::alignable<T> or std::constructible_from<std::size_t, T>);

} // namespace

// The devious shape: ==, < and their derived relations with no <=>, which totally_ordered admits and /9 does not.
struct no_spaceship
{
        std::size_t v = 0;

        constexpr no_spaceship() = default;
        // NOLINTNEXTLINE(misc-explicit-constructor, google-explicit-constructor)
        constexpr no_spaceship(std::size_t n) noexcept
            : v(n)
        {}

        [[nodiscard]] explicit constexpr operator std::size_t() const noexcept
        {
                return v;
        }

        [[nodiscard]] constexpr auto operator==(no_spaceship const&) const noexcept -> bool = default;

        [[nodiscard]] constexpr auto operator<(no_spaceship const& other) const noexcept -> bool
        {
                return v < other.v;
        }

        [[nodiscard]] constexpr auto operator>(no_spaceship const& other) const noexcept -> bool
        {
                return other.v < v;
        }

        [[nodiscard]] constexpr auto operator<=(no_spaceship const& other) const noexcept -> bool
        {
                return not(other.v < v);
        }

        [[nodiscard]] constexpr auto operator>=(no_spaceship const& other) const noexcept -> bool
        {
                return not(v < other.v);
        }

        [[nodiscard]] constexpr auto operator+(no_spaceship other) const noexcept -> no_spaceship
        {
                return {v + other.v};
        }

        [[nodiscard]] constexpr auto operator-(no_spaceship other) const noexcept -> no_spaceship
        {
                return {v - other.v};
        }

        [[nodiscard]] constexpr auto operator&(no_spaceship other) const noexcept -> no_spaceship
        {
                return {v & other.v};
        }
};

namespace xstd {

// What alignable asks of the type, borrowed whole from the integer it is a thin wrapper over.
template<>
struct numeric_limits<light> : std::numeric_limits<std::size_t>
{};

template<>
struct numeric_limits<no_spaceship> : std::numeric_limits<std::size_t>
{};

} // namespace xstd

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
        static_assert(not xstd::alignable<xstd::int128>);
#ifdef __BITINT_MAXWIDTH__
        static_assert(not xstd::alignable<signed _BitInt(24)>);
#endif
        // One bit wide, which is what the digits clause is for: bool's one plus one is still one.
        static_assert(not xstd::alignable<bool>);
        static_assert(xstd::numeric_limits<bool>::digits == 1);
        // Not an integer, and for the pointers not specialized at all, which is not_pointer for free.
        static_assert(not xstd::alignable<double>);
        static_assert(not xstd::alignable<void*>);
        static_assert(not xstd::alignable<std::uint8_t*>);
        // Nothing at all.
        struct udt
        {};
        static_assert(not xstd::alignable<udt>);
        static_assert(not xstd::alignable<void>);
        BOOST_CHECK(true);
}

// /9 is stated with three_way_comparable, so alignable asks it too: excluding that shape keeps this a prefix of it.
BOOST_AUTO_TEST_CASE(TheOrderingIsTheOneIntegerClassAsksFor)
{
        static_assert(std::totally_ordered<no_spaceship>);
        static_assert(not std::three_way_comparable<no_spaceship, std::strong_ordering>);
        static_assert(not xstd::alignable<no_spaceship>);

        // light has a defaulted <=>, which is why it stays in.
        static_assert(std::three_way_comparable<light, std::strong_ordering>);
        static_assert(xstd::alignable<light>);
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

        // And the converse fails, which is what makes it light rather than a synonym for unsigned_integer.
        static_assert(xstd::alignable<light>);
        static_assert(not xstd::unsigned_integer<light>);
        BOOST_CHECK(light{} == light{0});
        BOOST_CHECK((light{6} + light{1}) == light{7});
        BOOST_CHECK((light{6} - light{1}) == light{5});
        BOOST_CHECK((light{6} & light{3}) == light{2});
        BOOST_CHECK(light{6} >= light{5});
        BOOST_CHECK_EQUAL(static_cast<std::size_t>(light{6}), 6UZ);
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
        static_assert(xstd::alignable<light const volatile>);

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

        static_assert(not xstd::nothrow_alignable<light> or std::is_nothrow_move_constructible_v<light>);
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

        // light is alignable and not nothrow_alignable: its size_t constructor carries no noexcept.
        static_assert(xstd::alignable<light>);
        static_assert(not xstd::nothrow_alignable<light>);
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
