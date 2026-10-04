//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints.hpp>            // the whole ints surface
#include <boost/test/unit_test.hpp> // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <array>                    // array
#include <cstddef>                  // size_t
#include <format>                   // format
#include <string_view>              // string_view
#include <system_error>             // errc

BOOST_AUTO_TEST_SUITE(Ints)

// int128 is not the standard library's, so every module carries it alone; arriving whole is the invariant here.
BOOST_AUTO_TEST_CASE(TheWidestTypeReachesEveryModule)
{
        using T = xstd::int128;

        // concepts
        static_assert(xstd::signed_integer<T>);
        static_assert(xstd::unsigned_integer<xstd::make_unsigned_t<T>>);

        // limits, which std::numeric_limits alone would leave unspecialised
        static_assert(xstd::numeric_limits<T>::digits == 127);

        // cstdlib, over the type the standard library's div() cannot take
        BOOST_CHECK(xstd::abs(T{-2}) == T{2});
        BOOST_CHECK(xstd::div(T{8}, T{3}).quotient == T{2});

        // charconv, whose std counterpart has no overload this wide
        auto buffer       = std::array<char, xstd::to_chars_max_size<T>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), T{-42});
        BOOST_CHECK(result.ec == std::errc{});
        BOOST_CHECK(std::string_view(buffer.data(), result.ptr) == "-42");

        // format, over the aggregate cstdlib returns
        BOOST_CHECK_EQUAL(std::format("{}", xstd::div(T{8}, T{3})), "(2, 2)");
}

// The alignment functions take the unsigned half, and the widest one is still in it.
BOOST_AUTO_TEST_CASE(TheAlignmentFunctionsMeetTheConcepts)
{
        static_assert(xstd::alignable<std::size_t>);
        static_assert(xstd::alignable<xstd::uint128>);
        static_assert(not xstd::alignable<xstd::int128>);

        BOOST_CHECK(xstd::is_aligned(xstd::align_up(9UZ, 8), 8));
        BOOST_CHECK(xstd::is_aligned(xstd::align_down(9UZ, 8), 8));
        BOOST_CHECK(xstd::align_up(xstd::uint128{9}, 8) == xstd::uint128{16});
}

BOOST_AUTO_TEST_SUITE_END()
