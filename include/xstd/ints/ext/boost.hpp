//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_EXT_BOOST_HPP
#define XSTD_INTS_EXT_BOOST_HPP

// IWYU pragma: always_keep

// Boost's pair and Boost's hashing, and no umbrella above this one: an adapted library is asked for by name.
#include <xstd/ints/ext/boost/int128.hpp> // IWYU pragma: export; make_signed, make_unsigned
#include <xstd/ints/ext/boost/hash2.hpp>  // IWYU pragma: export; hash_append_int

#endif // XSTD_INTS_EXT_BOOST_HPP
