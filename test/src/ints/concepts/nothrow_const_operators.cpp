//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/concepts/integer.hpp>                 // NOLINT(misc-include-cleaner): used with optional Abseil fixtures
#include <xstd/ints/concepts/nothrow_const_operators.hpp> // nothrow_const_operators
#include <xstd/ints/cstdint.hpp>                          // XSTD_HAS_INT128, int128, uint128
#include <xstd/ints/cstdlib/abs.hpp>                      // abs
#include <xstd/ints/cstdlib/div.hpp>                      // div
#include <test/constexpr_check.hpp>                       // XSTD_CONSTEXPR_CHECK
#include <boost/test/unit_test.hpp>                       // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <complex>                                        // complex
#include <type_traits>                                    // is_nothrow_..._v

// Reached the way a consumer reaches it: the probe here, the pair inside the adapter.
#if __has_include(<absl/numeric/int128.h>)
#define TEST_HAS_ABSL_INT128
#include <xstd/ints/ext/absl/int128.hpp> // int128, uint128
#endif

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(NothrowConstOperators)

BOOST_AUTO_TEST_CASE(HoldsForTheTypesThatDeclareIt)
{
        // The built-in widths and the 128-bit type the library names all carry it.
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<int>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<char32_t>);

        // Its first conjunct is integer, which cv bool is not, so the operators go unasked.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<bool>);
#ifdef XSTD_HAS_INT128
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::int128> and xstd::nothrow_const_operators<xstd::uint128>);
#endif

#ifdef XSTD_HAS_BIT_INT
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::bit_int<17>>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::bit_uint<17>>);
#endif

        // Admitted by integer and refused here: why the two are separate concepts.
#ifdef TEST_HAS_ABSL_INT128
        XSTD_CONSTEXPR_CHECK(xstd::integer<absl::int128>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<absl::int128>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<absl::uint128>);
#endif
}

// The operators were never the whole cost: passing and returning by value spends /9's copy, move and destructor.
BOOST_AUTO_TEST_CASE(IncludesWhatPassingAndReturningCosts)
{
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_destructible_v<int>);
#ifdef XSTD_HAS_INT128
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_move_constructible_v<xstd::int128>);
        XSTD_CONSTEXPR_CHECK(std::is_nothrow_copy_constructible_v<xstd::uint128>);
#endif

        // Which the concept now answers for, over the types this file already names.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<int> or std::is_nothrow_copy_constructible_v<int>);
#ifdef TEST_HAS_ABSL_INT128
        using T = absl::int128;
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<T> or std::is_nothrow_copy_constructible_v<T>);
#endif
}

// Cv-transparent on both branches; volatile is the one that takes the stripping.
BOOST_AUTO_TEST_CASE(IsCvTransparentOnBothBranches)
{
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<int const>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<int volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<int const volatile>);

#ifdef XSTD_HAS_INT128
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::int128 const>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::int128 volatile>);
        XSTD_CONSTEXPR_CHECK(xstd::nothrow_const_operators<xstd::int128 const volatile>);
#endif

        // Nor does a qualifier turn a no into a yes.
#ifdef TEST_HAS_ABSL_INT128
        using T = absl::int128;

        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<T const>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<T volatile>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<T const volatile>);
#endif
}

// Total: the conjunction short-circuits on integer before any operator is named.
BOOST_AUTO_TEST_CASE(IsTotal)
{
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<void>);
        // remove_cv_t, not remove_cvref_t: a reference is not an integer type.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<int&>);
        // NOLINTNEXTLINE(modernize-avoid-c-arrays): a built-in array is the type under test, not a container choice
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<int[3]>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<int()>);
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<std::complex<double>>);

        // Turned away by integer rather than by ~ or %.
        XSTD_CONSTEXPR_CHECK(not xstd::nothrow_const_operators<double>);
}

// The concept is the exception specification, so the two must agree for every type.
BOOST_AUTO_TEST_CASE(IsTheExceptionSpecification)
{
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(1)) == xstd::nothrow_const_operators<int>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(1, 1)) == xstd::nothrow_const_operators<int>);

#ifdef TEST_HAS_ABSL_INT128
        using T = absl::int128;
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::abs(T{1})) == xstd::nothrow_const_operators<T>);
        XSTD_CONSTEXPR_CHECK(noexcept(xstd::div(T{1}, T{1})) == xstd::nothrow_const_operators<T>);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
