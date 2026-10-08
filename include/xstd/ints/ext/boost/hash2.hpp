//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_EXT_BOOST_HASH2_HPP
#define XSTD_INTS_EXT_BOOST_HASH2_HPP

#include <xstd/ints/concepts/integer.hpp>          // integer
#include <xstd/ints/limits/numeric_limits.hpp>     // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp>     // is_signed_v
#include <xstd/ints/type_traits/make_unsigned.hpp> // make_unsigned_t
#include <boost/hash2/hash_append.hpp>             // IWYU pragma: export; hash_append, the flavors
#include <cstdint>                                 // uint8_t, uint16_t, uint32_t, uint64_t
#include <limits>                                  // numeric_limits
#include <ranges>                                  // iota
#include <type_traits>                             // conditional_t, make_signed_t

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
                // Low word first, each a slice of the two's complement, and a part word on top sign- or zero-extended.
                auto bits = static_cast<make_unsigned_t<T>>(v);
                for ([[maybe_unused]] auto const i : std::views::iota(0, width / word_width)) {
                        boost::hash2::hash_append(h, f, static_cast<word>(bits));
                        // ">>" not ">>=": absl::uint128 is constexpr on the first only.
                        bits = bits >> unsigned{word_width};
                }
                constexpr auto top_width = width % word_width;
                if constexpr (top_width != 0) {
                        // Flipping the sign bit and subtracting it again sign-extends; unsigned, there is none to flip.
                        constexpr auto sign_bit = is_signed_v<T> ? word{1} << unsigned{top_width - 1} : word{0};
                        auto const top_word     = static_cast<word>(bits);
                        boost::hash2::hash_append(h, f, (top_word ^ sign_bit) - sign_bit);
                }
        }
}

} // namespace xstd

#endif // XSTD_INTS_EXT_BOOST_HASH2_HPP
