//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_EXT_BOOST_HASH2_HPP
#define XSTD_INTS_EXT_BOOST_HASH2_HPP

#include <xstd/ints/concepts/integer.hpp>      // integer
#include <xstd/ints/limits/numeric_limits.hpp> // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp> // is_signed_v
#include <boost/hash2/hash_append.hpp>         // IWYU pragma: export; hash_append, the flavors
#include <cstdint>                             // uint8_t, uint16_t, uint32_t, uint64_t
#include <limits>                              // numeric_limits
#include <type_traits>                         // conditional_t, make_signed_t

namespace xstd {

// The value, sign- or zero-extended to the narrowest standard width that holds it, and past 64 bits to whole words.
template<class Hash, class Flavor, integer T>
constexpr auto hash_append_int(Hash& h, Flavor const& f, T v)
        -> void
{
        using word                = std::uint64_t;
        constexpr auto word_width = std::numeric_limits<word>::digits;
        constexpr auto width      = xstd::numeric_limits<T>::digits + (is_signed_v<T> ? 1 : 0);
        if constexpr (width <= word_width) {
                using narrowest_unsigned = std::conditional_t<
                        width <= std::numeric_limits<std::uint8_t>::digits, std::uint8_t,
                        std::conditional_t<
                                width <= std::numeric_limits<std::uint16_t>::digits, std::uint16_t,
                                std::conditional_t<width <= std::numeric_limits<std::uint32_t>::digits, std::uint32_t, word>>>;
                using narrowest = std::conditional_t<is_signed_v<T>, std::make_signed_t<narrowest_unsigned>, narrowest_unsigned>;
                boost::hash2::hash_append(h, f, static_cast<narrowest>(v));
        } else {
                // Low word first; an arithmetic shift leaves a negative value's top word sign-extended.
                for (auto shift = 0; shift < width; shift += word_width) {
                        boost::hash2::hash_append(h, f, static_cast<word>(v >> shift));
                }
        }
}

} // namespace xstd

#endif // XSTD_INTS_EXT_BOOST_HASH2_HPP
