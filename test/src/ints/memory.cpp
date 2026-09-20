//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/memory.hpp>       // align_down, align_up, is_aligned
#include <test/constexpr_check.hpp>   // XSTD_CONSTEXPR_CHECK
#include <boost/align/align_down.hpp> // align_down
#include <boost/align/is_aligned.hpp> // is_aligned
#include <boost/align/align_up.hpp>   // align_up
#include <boost/test/unit_test.hpp>   // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <array>                      // array
#include <concepts>                   // same_as
#include <cstddef>                    // size_t
#include <cstdint>                    // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Memory)

BOOST_AUTO_TEST_CASE(AgreesWithBoostAlignment)
{
        // clang-format off
        XSTD_CONSTEXPR_CHECK(xstd::align_up  ( 0UZ, 8) == boost::alignment::align_up  ( 0UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_up  ( 1UZ, 8) == boost::alignment::align_up  ( 1UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_up  ( 9UZ, 8) == boost::alignment::align_up  ( 9UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_up  (65UZ, 8) == boost::alignment::align_up  (65UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_down( 0UZ, 8) == boost::alignment::align_down( 0UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_down( 1UZ, 8) == boost::alignment::align_down( 1UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_down( 9UZ, 8) == boost::alignment::align_down( 9UZ, 8));
        XSTD_CONSTEXPR_CHECK(xstd::align_down(65UZ, 8) == boost::alignment::align_down(65UZ, 8));
        // clang-format on
}

// Every value and power-of-two alignment below the width where a result still fits.
template<class I>
auto agrees_with_boost_align()
        -> void
{
        for (auto alignment = 1UZ; alignment <= 16UZ; alignment *= 2UZ) {
                for (auto value = 0UZ; value < 48UZ; ++value) {
                        auto const x = static_cast<I>(value);
                        BOOST_CHECK_EQUAL(static_cast<std::size_t>(xstd::align_up(x, alignment)),
                                          static_cast<std::size_t>(boost::alignment::align_up(x, alignment)));
                        BOOST_CHECK_EQUAL(static_cast<std::size_t>(xstd::align_down(x, alignment)),
                                          static_cast<std::size_t>(boost::alignment::align_down(x, alignment)));
                }
        }
}

BOOST_AUTO_TEST_CASE(AgreesWithBoostAlignmentAtEveryWidth)
{
        agrees_with_boost_align<std::uint8_t>();
        agrees_with_boost_align<std::uint16_t>();
        agrees_with_boost_align<std::uint32_t>();
        agrees_with_boost_align<std::uint64_t>();
        agrees_with_boost_align<std::size_t>();
}

// What align_up and align_down leave behind is aligned, and what they are given is left alone once it is.
namespace {

auto agrees_with_the_other_two(std::size_t value, std::size_t alignment)
        -> void
{
        BOOST_CHECK(xstd::is_aligned(xstd::align_up(value, alignment), alignment));
        BOOST_CHECK(xstd::is_aligned(xstd::align_down(value, alignment), alignment));
        BOOST_CHECK_EQUAL(xstd::is_aligned(value, alignment), xstd::align_down(value, alignment) == value);
        BOOST_CHECK_EQUAL(xstd::is_aligned(value, alignment), boost::alignment::is_aligned(value, alignment));
}

} // namespace

BOOST_AUTO_TEST_CASE(ThePredicateAgreesWithTheTwoAdjustments)
{
        for (auto alignment = 1UZ; alignment <= 64UZ; alignment *= 2UZ) {
                for (auto value = 0UZ; value < 256UZ; ++value) {
                        agrees_with_the_other_two(value, alignment);
                }
        }
}

namespace {

auto agrees_with_boost_pointers(char* p, std::size_t alignment)
        -> void
{
        BOOST_CHECK_EQUAL(static_cast<void*>(xstd::align_up(p, alignment)),
                          boost::alignment::align_up(static_cast<void*>(p), alignment));
        BOOST_CHECK_EQUAL(static_cast<void*>(xstd::align_down(p, alignment)),
                          boost::alignment::align_down(static_cast<void*>(p), alignment));
        BOOST_CHECK_EQUAL(xstd::is_aligned(p, alignment),
                          boost::alignment::is_aligned(static_cast<void const*>(p), alignment));
}

} // namespace

// The pointer overloads are the same arithmetic in the address space, so they answer as Boost's do.
BOOST_AUTO_TEST_CASE(PointerOverloadsAgreeWithBoostAlignment)
{
        alignas(4096) static std::array<char, 8192> buffer;
        for (auto offset = 0UZ; offset < 512UZ; ++offset) {
                for (auto alignment = 1UZ; alignment <= 256UZ; alignment *= 2UZ) {
                        agrees_with_boost_pointers(buffer.data() + offset, alignment);
                }
        }
}

// The pointee type survives, where Boost hands back void* and leaves the cast to the caller.
BOOST_AUTO_TEST_CASE(PointerOverloadsKeepThePointeeType)
{
        alignas(64) static std::array<int, 16> buffer;
        static_assert(std::same_as<decltype(xstd::align_up(buffer.data(), 64)), int*>);
        static_assert(std::same_as<decltype(xstd::align_down(buffer.data(), 64)), int*>);
        static_assert(std::same_as<decltype(xstd::is_aligned(buffer.data(), 64)), bool>);
        BOOST_CHECK(xstd::is_aligned(buffer.data(), 64));
        BOOST_CHECK_EQUAL(xstd::align_up(buffer.data(), 64), buffer.data());
        BOOST_CHECK_EQUAL(xstd::align_down(buffer.data(), 64), buffer.data());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
