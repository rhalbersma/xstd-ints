//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/bit/detail/bit_block.hpp>  // bit_block
#include <xstd/ints/bit/detail/intrinsic.hpp>  // IWYU pragma: keep; XSTD_INTS_HAS_FAST_BMI2, read only by the preprocessor
#include <xstd/ints/cstdint.hpp>               // uint128
#include <xstd/ints/limits.hpp>                // numeric_limits
#include <test/bit/permutation_properties.hpp> // intrinsics_agree, permutations_hold, portable_kernels_round_trip
#include <test/bit/shift_count.hpp>            // shift_count_t
#include <test/bit/sweep.hpp>                  // bit_precise_sweep_types
#include <cstddef>                             // size_t
#include <cstdint>                             // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstdlib>                             // abort
#include <ranges>                              // iota
#include <span>                                // span
#include <tuple>                               // apply, tuple, tuple_cat
#include <utility>                             // declval

namespace {

// Every width the public functions take: one block, a part of one, two, and the bit-precise ones across block boundaries.
using fuzzed_types = decltype(std::tuple_cat(
        std::declval<std::tuple<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, xstd::uint128>>(),
        std::declval<test::bit_precise_sweep_types>()
));

// Little-endian bytes from offset, as many as T is wide, reading zero past the end of the input.
template<class T>
[[nodiscard]] auto read_at(std::span<std::uint8_t const> input, std::size_t offset)
        -> T
{
        constexpr auto bytes = (static_cast<std::size_t>(xstd::numeric_limits<T>::digits) + 7UZ) / 8UZ;
        auto value           = T{0};
        // Each byte shifted into place rather than the value shifted past it: a _BitInt(2) has no room to shift by 8.
        for (auto const k : std::views::iota(0UZ, bytes)) {
                auto const byte = offset + k < input.size() ? input[offset + k] : std::uint8_t{0};
                value           = static_cast<T>(value | static_cast<T>(static_cast<T>(byte) << static_cast<test::shift_count_t<T>>(8UZ * k)));
        }
        return value;
}

// The source from the input's first bytes and the mask from those after it, so every width reads the same prefix.
template<class T>
[[nodiscard]] auto holds_at(std::span<std::uint8_t const> input)
        -> bool
{
        constexpr auto bytes = (static_cast<std::size_t>(xstd::numeric_limits<T>::digits) + 7UZ) / 8UZ;
        return test::permutations_hold(read_at<T>(input, 0UZ), read_at<T>(input, bytes));
}

auto check(bool holds)
        -> void
{
        if (not holds) {
                std::abort();
        }
}

} // namespace

extern "C" auto LLVMFuzzerTestOneInput(std::uint8_t const* data, std::size_t size)
        -> int
{
        using xstd::ints::detail::bit_block;
        auto const input = std::span(data, size);
        auto const src   = read_at<bit_block>(input, 0UZ);
        auto const mask  = read_at<bit_block>(input, sizeof(bit_block));
        check(test::portable_kernels_round_trip(src, mask));
#ifdef XSTD_INTS_HAS_FAST_BMI2
        check(test::intrinsics_agree(src, mask));
#endif
        std::apply([&]<class... T>(T...) -> void { (check(holds_at<T>(input)), ...); }, fuzzed_types{});
        return 0;
}
