//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdlib.hpp>                   // complete arithmetic surface
#include <xstd/ints/cstdint.hpp>                   // XSTD_HAS_BIT_INT, bit_int, bit_uint
#include <xstd/ints/limits.hpp>                    // numeric_limits
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <test/boost_test_print_log_value.hpp>     // NOLINT(misc-include-cleaner): registers Boost.Test printers
#include <test/constexpr_check.hpp>                // XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>              // exact-width integer type lists, bit-precise type lists
#include <boost/test/unit_test.hpp>                // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(CStdLib)

// Without negative values, truncated, floored and Euclidean division are the same operation.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedDivisionConventionsAgree, T, test::exact_width_unsigned_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{8}, T{3})), (xstd::div_euclid(T{8}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{8}, T{3})), (xstd::div_floor(T{8}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{1}, T{2})), (xstd::div_euclid(T{1}, T{2})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{1}, T{2})), (xstd::div_floor(T{1}, T{2})));

        using limits = xstd::numeric_limits<T>;
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>,
                                      (xstd::div(limits::max(), T{3})), (xstd::div_euclid(limits::max(), T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>,
                                      (xstd::div(limits::max(), T{3})), (xstd::div_floor(limits::max(), T{3})));
}

// An exact division has no convention-dependent remainder, even for signed operands.
BOOST_AUTO_TEST_CASE_TEMPLATE(ExactDivisionConventionsAgree, T, test::exact_width_signed_integer_types)
{
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-6}, T{3})), (xstd::div_euclid(T{-6}, T{3})));
        XSTD_CONSTEXPR_CHECK_EQUAL_IF(test::has_constexpr_division<T>, (xstd::div(T{-6}, T{3})), (xstd::div_floor(T{-6}, T{3})));
}

// The postconditions of all six over every value a narrow width holds: the one place the whole surface can be swept.
#ifdef XSTD_HAS_BIT_INT

namespace {

// The identity reconstructed in int: div_floor(1, -2) gives -1, and -1 * -2 is 2, which _BitInt(2) cannot hold.
template<class T>
auto reconstructs(T quotient, T denom, T remainder, T numer)
        -> bool
{
        return (static_cast<int>(quotient) * static_cast<int>(denom)) + static_cast<int>(remainder) == static_cast<int>(numer);
}

// Truncated: the quotient identity, the remainder bound, and the numerator's sign.
template<class T>
auto check_truncated(T numer, T denom)
        -> void
{
        auto const [quotient, remainder] = xstd::div(numer, denom);
        BOOST_CHECK(reconstructs(quotient, denom, remainder, numer));
        BOOST_CHECK(xstd::unsigned_abs(remainder) < xstd::unsigned_abs(denom));
        BOOST_CHECK(remainder == T(0) or xstd::sign(remainder) == xstd::sign(numer));
}

// Floored: the same identity, and a nonzero remainder carrying the denominator's sign.
template<class T>
auto check_floored(T numer, T denom)
        -> void
{
        auto const [quotient, remainder] = xstd::div_floor(numer, denom);
        BOOST_CHECK(reconstructs(quotient, denom, remainder, numer));
        BOOST_CHECK(remainder == T(0) or xstd::sign(remainder) == xstd::sign(denom));
}

// Euclidean: the same identity, and a remainder that is never negative.
template<class T>
auto check_euclidean(T numer, T denom)
        -> void
{
        auto const [quotient, remainder] = xstd::div_euclid(numer, denom);
        BOOST_CHECK(reconstructs(quotient, denom, remainder, numer));
        BOOST_CHECK(xstd::sign(remainder) >= 0);
}

// Every ordered pair the type can hold, which only a width this narrow makes affordable.
template<class T>
auto sweep()
        -> void
{
        using limits = xstd::numeric_limits<T>;
        auto const lo = static_cast<int>(limits::min());
        auto const hi = static_cast<int>(limits::max());
        for (auto n = lo; n <= hi; ++n) {
                for (auto d = lo; d <= hi; ++d) {
                        // The two the contract excludes, rather than the two it gets wrong.
                        if (d == 0 or (n == lo and d == -1)) {
                                continue;
                        }
                        check_truncated(T(n), T(d));
                        check_floored(T(n), T(d));
                        check_euclidean(T(n), T(d));
                }
        }
}

} // namespace

// What no other width affords: the postconditions over the complete value space, not a sample.
BOOST_AUTO_TEST_CASE(DivisionOverEveryPair)
{
        sweep<xstd::bit_uint<2>>();
        sweep<xstd::bit_uint<3>>();
        sweep<xstd::bit_uint<4>>();
        sweep<xstd::bit_int<2>>();
        sweep<xstd::bit_int<3>>();
        sweep<xstd::bit_int<4>>();
}

// And at every width in the list, the values a sweep of 256 bits could never reach.
BOOST_AUTO_TEST_CASE_TEMPLATE(SignedExtremes, T, test::bit_precise_signed_types)
{
        using limits = xstd::numeric_limits<T>;
        using U = xstd::make_unsigned_t<T>;

        BOOST_CHECK(xstd::sign(limits::min()) == -1);
        BOOST_CHECK(xstd::sign(limits::max()) == +1);
        BOOST_CHECK(xstd::abs(static_cast<T>(limits::min() + T(1))) == limits::max());

        // The one magnitude no signed width holds, which is why the counterpart returns it.
        BOOST_CHECK(xstd::unsigned_abs(limits::min()) == static_cast<U>(limits::max()) + U(1));
}

// The unsigned extremes at every configured native width.
BOOST_AUTO_TEST_CASE_TEMPLATE(UnsignedExtremes, T, test::bit_precise_unsigned_types)
{
        using limits = xstd::numeric_limits<T>;

        BOOST_CHECK(xstd::sign(limits::min()) == 0);
        BOOST_CHECK(xstd::sign(limits::max()) == +1);
        BOOST_CHECK(xstd::abs(limits::max()) == limits::max());
        BOOST_CHECK(xstd::unsigned_abs(limits::max()) == limits::max());
        BOOST_CHECK(xstd::div(limits::max(), limits::max()).quotient == T(1));
}

#else

// The lists are empty without the storage, and Boost.Test wants a case either way.
BOOST_AUTO_TEST_CASE(AbsentWithoutCompilerSupport)
{
        BOOST_CHECK(std::tuple_size_v<test::bit_precise_signed_types> == 0);
}

#endif // XSTD_HAS_BIT_INT

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
