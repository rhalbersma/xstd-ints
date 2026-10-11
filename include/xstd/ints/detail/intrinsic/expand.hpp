//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_INTS_DETAIL_INTRINSIC_EXPAND_HPP
#define XSTD_INTS_DETAIL_INTRINSIC_EXPAND_HPP

#include <xstd/ints/detail/bit_limb.hpp>       // bit_limb
#include <xstd/ints/detail/intrinsic/bmi2.hpp> // XSTD_INTS_HAS_FAST_BMI2

#ifdef XSTD_INTS_HAS_FAST_BMI2

namespace xstd::ints::detail::intrinsic {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters): P3104's signature, the value and then the mask
[[nodiscard]] inline auto expand(bit_limb x, bit_limb m) noexcept
        -> bit_limb
{
        return static_cast<bit_limb>(_pdep_u64(x, m));
}

} // namespace xstd::ints::detail::intrinsic

#endif

#endif // XSTD_INTS_DETAIL_INTRINSIC_EXPAND_HPP
