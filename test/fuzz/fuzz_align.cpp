//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint.hpp>           // bit_uint
#include <xstd/ints/limits.hpp>            // numeric_limits
#include <xstd/ints/memory/align_down.hpp> // align_down
#include <xstd/ints/memory/align_up.hpp>   // align_up
#include <xstd/ints/memory/is_aligned.hpp> // is_aligned
#include <fuzz/input.hpp>                  // check, operand, trailer_at, unsigned_types
#include <cstddef>                         // size_t
#include <cstdint>                         // uint8_t
#include <limits>                          // numeric_limits
#include <span>                            // span
#include <tuple>                           // apply

namespace {

// Wide enough that a value plus an alignment never wraps, so whether align_up's answer fits is a comparison.
using wide = xstd::bit_uint<256>;

template<class T>
auto check_at(T value, std::size_t alignment)
        -> void
{
        auto const remainder = static_cast<wide>(static_cast<wide>(value) % static_cast<wide>(alignment));
        auto const down      = static_cast<wide>(static_cast<wide>(value) - remainder);
        auto const up        = remainder == 0U ? down : static_cast<wide>(down + static_cast<wide>(alignment));
        fuzz::check(xstd::is_aligned(value, alignment) == (remainder == 0U));
        fuzz::check(static_cast<wide>(xstd::align_down(value, alignment)) == down);
        // An alignment wider than T rounds every value but zero past the maximum, which the precondition rules out.
        if (up <= static_cast<wide>(xstd::numeric_limits<T>::max())) {
                fuzz::check(static_cast<wide>(xstd::align_up(value, alignment)) == up);
        }
}

} // namespace

extern "C" auto LLVMFuzzerTestOneInput(std::uint8_t const* data, std::size_t size)
        -> int
{
        auto const input     = std::span(data, size);
        auto const exponent  = fuzz::trailer_at(input, 1UZ, 0UZ) % std::numeric_limits<std::size_t>::digits;
        auto const alignment = std::size_t{1} << exponent;
        std::apply([&]<class... T>(T...) -> void { (check_at(fuzz::operand<T>(input, 0UZ), alignment), ...); }, fuzz::unsigned_types{});
        return 0;
}
