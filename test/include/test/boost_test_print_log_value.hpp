//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP
#define TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP

#include <xstd/ints/charconv/to_chars.hpp>  // to_chars, to_chars_max_size
#include <xstd/ints/concepts/integer.hpp>   // integer
#include <xstd/ints/cstdint.hpp>            // int128, uint128
#include <xstd/ints/cstdlib/div_result.hpp> // div_result
#include <boost/test/unit_test.hpp>         // print_log_value
#include <array>                            // array
#include <cassert>                          // assert
#include <cstddef>                          // size_t
#include <ostream>                          // ostream
#include <string_view>                      // string_view
#include <system_error>                     // errc

namespace test {

template<xstd::integer I>
auto print_integer(std::ostream& ostr, I const value)
        -> void
{
        auto buffer = std::array<char, xstd::to_chars_max_size<I>>{};
        auto const result = xstd::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
        assert(result.ec == std::errc{});
        ostr << std::string_view(buffer.data(), result.ptr);
}

} // namespace test

namespace boost::test_tools::tt_detail {

template<>
struct print_log_value<xstd::int128>
{
        auto operator()(std::ostream& ostr, xstd::int128 const value) const
                -> void
        {
                test::print_integer(ostr, value);
        }
};

template<>
struct print_log_value<xstd::uint128>
{
        auto operator()(std::ostream& ostr, xstd::uint128 const value) const
                -> void
        {
                test::print_integer(ostr, value);
        }
};

#ifdef XSTD_HAS_BIT_INT

template<std::size_t N>
struct print_log_value<signed _BitInt(N)>
{
        auto operator()(std::ostream& ostr, signed _BitInt(N) const value) const
                -> void
        {
                test::print_integer(ostr, value);
        }
};

template<std::size_t N>
struct print_log_value<unsigned _BitInt(N)>
{
        auto operator()(std::ostream& ostr, unsigned _BitInt(N) const value) const
                -> void
        {
                test::print_integer(ostr, value);
        }
};

#endif // XSTD_HAS_BIT_INT

template<xstd::integer I>
struct print_log_value<xstd::div_result<I>>
{
        auto operator()(std::ostream& ostr, xstd::div_result<I> const& d) const
                -> void
        {
                ostr << '(';
                test::print_integer(ostr, d.quotient);
                ostr << ", ";
                test::print_integer(ostr, d.remainder);
                ostr << ')';
        }
};

} // namespace boost::test_tools::tt_detail

#endif // TEST_BOOST_TEST_PRINT_LOG_VALUE_HPP
