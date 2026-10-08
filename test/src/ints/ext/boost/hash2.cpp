//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/ints/cstdint.hpp>               // XSTD_HAS_BIT_INT, bit_int, bit_uint, int128, uint128
#include <xstd/ints/limits.hpp>                // numeric_limits
#include <xstd/ints/type_traits/is_signed.hpp> // is_signed_v
#include <test/constexpr_check.hpp>            // XSTD_CONSTEXPR_CHECK, XSTD_CONSTEXPR_CHECK_EQUAL
#include <test/exact_width_types.hpp>          // absl_signed_types, absl_unsigned_types, boost_signed_types, boost_unsigned_types, exact_width_integer_types
#include <boost/test/unit_test.hpp>            // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <algorithm>                           // max
#include <array>                               // array
#include <bit>                                 // bit_ceil, endian
#include <cstddef>                             // size_t
#include <cstdint>                             // int8_t, int16_t, int32_t, int64_t, uint16_t, uint32_t, uint64_t
#include <cstring>                             // memcpy
#include <ranges>                              // iota
#include <tuple>                               // tuple, tuple_cat
#include <type_traits>                         // conditional_t
#include <utility>                             // declval

// Reached the way a consumer reaches it: the probe here, the adapter behind it.
#if __has_include(<boost/hash2/hash_append.hpp>)
#define TEST_HAS_BOOST_HASH2
#include <xstd/ints/ext/boost/hash2.hpp> // hash_append_int
#include <boost/hash2/flavor.hpp>        // big_endian_flavor, default_flavor, little_endian_flavor
#include <boost/hash2/fnv1a.hpp>         // fnv1a_64
#endif

BOOST_AUTO_TEST_SUITE(Ints)
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_AUTO_TEST_SUITE(Boost)
BOOST_AUTO_TEST_SUITE(Hash2)

#ifdef TEST_HAS_BOOST_HASH2

namespace {

// The bytes a hash would be fed, kept rather than digested, so a case reads back what the writer chose.
struct message
{
        std::array<unsigned char, 32> bytes{};
        std::size_t size{};

        constexpr auto update(unsigned char const* p, std::size_t n)
                -> void
        {
                for (auto const i : std::views::iota(0UZ, n)) {
                        bytes.at(size++) = p[i];
                }
        }

        // Hash2 hands a contiguously hashable value over as its object, outside constant evaluation.
        auto update(void const* p, std::size_t n)
                -> void
        {
                update(static_cast<unsigned char const*>(p), n);
        }

        [[nodiscard]] friend auto operator==(message const&, message const&) -> bool = default;
};

template<class Flavor = boost::hash2::little_endian_flavor, class T>
[[nodiscard]] constexpr auto message_of(T v)
        -> message
{
        auto m = message{};
        xstd::hash_append_int(m, Flavor{}, v);
        return m;
}

template<class Flavor = boost::hash2::little_endian_flavor, class T>
[[nodiscard]] constexpr auto digest_of(T v)
        -> std::uint64_t
{
        auto h = boost::hash2::fnv1a_64{};
        xstd::hash_append_int(h, Flavor{}, v);
        return h.result();
}

template<class T>
inline constexpr auto width = static_cast<std::size_t>(xstd::numeric_limits<T>::digits) + (xstd::is_signed_v<T> ? 1UZ : 0UZ);

// The contract restated in bytes: the narrowest standard width that holds the value, and past 64 bits whole words.
template<class T>
inline constexpr auto message_size = width<T> <= 64UZ ? std::bit_ceil(std::max(width<T>, 8UZ)) / 8UZ : (width<T> + 63UZ) / 64UZ * 8UZ;

// The little-endian message whose bit k is set where the predicate says, built from bytes and never from a T.
template<class T, class Predicate>
[[nodiscard]] constexpr auto expected_bits(Predicate is_set)
        -> message
{
        auto m = message{};
        m.size = message_size<T>;
        for (auto const k : std::views::iota(0UZ, 8UZ * message_size<T>)) {
                if (is_set(k)) {
                        m.bytes.at(k / 8UZ) |= static_cast<unsigned char>(1U << (k % 8UZ));
                }
        }
        return m;
}

// Byte i holds i + 1 for every byte below the sign bit, so a byte or a word read from the wrong place shows.
template<class T>
inline constexpr auto ascending_size = (width<T> - 1UZ) / 8UZ;

template<class T>
[[nodiscard]] constexpr auto ascending_value()
        -> T
{
        auto v = T{0};
        if constexpr (ascending_size<T> > 0UZ) {
                for (auto i = ascending_size<T> - 1UZ; i < ascending_size<T>; --i) {
                        v = static_cast<T>(static_cast<T>(v << 8U) | static_cast<T>(i + 1UZ));
                }
        }
        return v;
}

template<class T>
[[nodiscard]] constexpr auto ascending_message()
        -> message
{
        auto m = message{};
        m.size = message_size<T>;
        for (auto const i : std::views::iota(0UZ, ascending_size<T>)) {
                m.bytes.at(i) = static_cast<unsigned char>(i + 1UZ);
        }
        return m;
}

// Zero, one, the extremes, minus one, and the ascending bytes, each against the message its bits spell.
template<class T>
[[nodiscard]] constexpr auto is_its_little_endian_bytes()
        -> bool
{
        constexpr auto w = width<T>;
        using limits     = xstd::numeric_limits<T>;

        auto matches = message_of(T{0}) == expected_bits<T>([](std::size_t) -> bool { return false; }) and
                       message_of(T{1}) == expected_bits<T>([](std::size_t k) -> bool { return k == 0UZ; }) and
                       message_of(ascending_value<T>()) == ascending_message<T>();
        if constexpr (xstd::is_signed_v<T>) {
                matches = matches and
                          message_of(static_cast<T>(-1)) == expected_bits<T>([](std::size_t) -> bool { return true; }) and
                          message_of(limits::min()) == expected_bits<T>([](std::size_t k) -> bool { return k >= w - 1UZ; }) and
                          message_of(limits::max()) == expected_bits<T>([](std::size_t k) -> bool { return k < w - 1UZ; });
        } else {
                matches = matches and
                          message_of(limits::max()) == expected_bits<T>([](std::size_t k) -> bool { return k < w; });
        }
        return matches;
}

// Widths that fill no standard type, around each of the first two word boundaries, as far as the build reaches.
#ifdef XSTD_HAS_BIT_INT
using odd_bit_precise_narrow_types = std::tuple<xstd::bit_int<2>, xstd::bit_uint<2>, xstd::bit_int<24>, xstd::bit_uint<24>>;
#if __BITINT_MAXWIDTH__ >= 129
using odd_bit_precise_wide_types = std::tuple<xstd::bit_int<65>, xstd::bit_uint<65>, xstd::bit_int<100>, xstd::bit_uint<100>, xstd::bit_int<129>, xstd::bit_uint<129>>;
#else
using odd_bit_precise_wide_types = std::tuple<>;
#endif
#else
using odd_bit_precise_narrow_types = std::tuple<>;
using odd_bit_precise_wide_types   = std::tuple<>;
#endif

using hashed_types = decltype(std::tuple_cat(
        std::declval<test::exact_width_integer_types>(), std::declval<odd_bit_precise_narrow_types>(),
        std::declval<odd_bit_precise_wide_types>()
));

// Every 128-bit type the build has, which must all agree byte for byte.
#if defined(XSTD_HAS_BIT_INT) and __BITINT_MAXWIDTH__ >= 128
using bit_precise_128_types = std::tuple<xstd::bit_int<128>, xstd::bit_uint<128>>;
#else
using bit_precise_128_types = std::tuple<>;
#endif

using types_128 = decltype(std::tuple_cat(
        std::declval<std::tuple<xstd::int128, xstd::uint128>>(), std::declval<test::boost_signed_types>(),
        std::declval<test::boost_unsigned_types>(), std::declval<test::absl_signed_types>(),
        std::declval<test::absl_unsigned_types>(), std::declval<bit_precise_128_types>()
));

template<class T>
[[nodiscard]] constexpr auto from_words(std::uint64_t low, std::uint64_t high)
        -> T
{
        return static_cast<T>(static_cast<T>(static_cast<T>(high) << 64U) | static_cast<T>(low));
}

} // namespace

// The whole contract, for every type the suite carries and at run time, where Hash2 takes a value as its object.
BOOST_AUTO_TEST_CASE_TEMPLATE(EveryValueIsItsLittleEndianBytes, T, hashed_types)
{
        BOOST_CHECK(is_its_little_endian_bytes<T>());
}

// And in constant evaluation, where Hash2 writes every value byte by byte instead.
BOOST_AUTO_TEST_CASE_TEMPLATE(AndSoInConstantEvaluation, T, hashed_types)
{
        static_assert(is_its_little_endian_bytes<T>());
        BOOST_CHECK(true);
}

// Pinned against FNV-1a computed outside this library over the 16 bytes each value spells.
BOOST_AUTO_TEST_CASE_TEMPLATE(Every128BitTypeGivesTheSameDigests, T, types_128)
{
        // 00 01 02 .. 0f: the low word first, each word in little-endian order.
        XSTD_CONSTEXPR_CHECK_EQUAL(digest_of(from_words<T>(0x0706'0504'0302'0100U, 0x0f0e'0d0c'0b0a'0908U)), 0x7c84'dc94'7785'1775U);

        // 07 06 .. 00 0f 0e .. 08: still the low word first, each word now in big-endian order.
        XSTD_CONSTEXPR_CHECK_EQUAL(digest_of<boost::hash2::big_endian_flavor>(from_words<T>(0x0706'0504'0302'0100U, 0x0f0e'0d0c'0b0a'0908U)), 0xc389'ad53'4d5a'eaa5U);

        // Sixteen ff bytes, which is -1 and the unsigned maximum alike.
        XSTD_CONSTEXPR_CHECK_EQUAL(digest_of(static_cast<T>(-1)), 0xd660'7508'f5a1'e855U);

        // Fifteen zero bytes and 80, which is the signed minimum and the unsigned top bit alike.
        XSTD_CONSTEXPR_CHECK_EQUAL(digest_of(from_words<T>(0U, 0x8000'0000'0000'0000U)), 0x881f'9fb9'60fe'8ae5U);
}

BOOST_AUTO_TEST_CASE(StandardNamesHashAsTheExactWidthTypeOfTheirWidth)
{
        // 01 00 00 00 00 00 00 00, pinned against FNV-1a computed outside this library.
        XSTD_CONSTEXPR_CHECK_EQUAL(digest_of(std::uint64_t{1}), 0x89cd'3129'1d2a'efa4U);
        XSTD_CONSTEXPR_CHECK_EQUAL(digest_of(std::int64_t{1}), 0x89cd'3129'1d2a'efa4U);

        // Every name for a standard width hashes as the exact-width type of that width.
        using same_width_as_long = std::conditional_t<sizeof(long) == sizeof(std::int64_t), std::int64_t, std::int32_t>;
        XSTD_CONSTEXPR_CHECK(message_of(-2L) == message_of(same_width_as_long{-2}));
        XSTD_CONSTEXPR_CHECK(message_of(-2LL) == message_of(std::int64_t{-2}));
        XSTD_CONSTEXPR_CHECK(message_of(2ULL) == message_of(std::uint64_t{2}));
        XSTD_CONSTEXPR_CHECK(message_of(short{-2}) == message_of(std::int16_t{-2}));
}

#ifdef XSTD_HAS_BIT_INT

// A bit-precise integer of a standard width hashes as that standard type, and a narrower one as the next one up.
BOOST_AUTO_TEST_CASE(BitPreciseHashesAsTheStandardTypeThatHoldsIt)
{
        XSTD_CONSTEXPR_CHECK(message_of(xstd::bit_int<8>{-128}) == message_of(std::int8_t{-128}));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::bit_uint<16>{0xbeefU}) == message_of(std::uint16_t{0xbeefU}));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::bit_int<32>{-7}) == message_of(std::int32_t{-7}));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::bit_int<64>{-7}) == message_of(std::int64_t{-7}));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::numeric_limits<xstd::bit_uint<64>>::max()) == message_of(xstd::numeric_limits<std::uint64_t>::max()));
        XSTD_CONSTEXPR_CHECK(digest_of(xstd::bit_uint<64>{1}) == digest_of(std::uint64_t{1}));

        // Signed extends with ones and unsigned with zeros, so -1 and the maximum part company at 24 bits.
        XSTD_CONSTEXPR_CHECK(message_of(xstd::bit_int<24>{-1}) == message_of(std::int32_t{-1}));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::numeric_limits<xstd::bit_uint<24>>::max()) == message_of(std::uint32_t{0x00ff'ffffU}));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::numeric_limits<xstd::bit_int<24>>::min()) == message_of(std::int32_t{-0x80'0000}));
}

#if __BITINT_MAXWIDTH__ >= 128

// At 100 bits, a bit-precise integer spans the two words a 128-bit type does, and agrees with it value for value.
BOOST_AUTO_TEST_CASE(BitPreciseAboveAWordHashesAsWholeWords)
{
        using S = xstd::bit_int<100>;
        using U = xstd::bit_uint<100>;

        XSTD_CONSTEXPR_CHECK(message_of(S{-1}) == message_of(xstd::int128{-1}));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::numeric_limits<S>::min()) == message_of(from_words<xstd::int128>(0U, 0xffff'fff8'0000'0000U)));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::numeric_limits<S>::max()) == message_of(from_words<xstd::int128>(~0ULL, 0x0000'0007'ffff'ffffU)));
        XSTD_CONSTEXPR_CHECK(message_of(xstd::numeric_limits<U>::max()) == message_of(from_words<xstd::uint128>(~0ULL, 0x0000'000f'ffff'ffffU)));

        // Zero-extended, the unsigned maximum is not the all-ones message -1 is.
        XSTD_CONSTEXPR_CHECK(message_of(xstd::numeric_limits<U>::max()) != message_of(S{-1}));
}

#endif

// Only the value is hashed: a bit_int<24> carries a byte of padding, and what that byte holds never reaches a message.
BOOST_AUTO_TEST_CASE(PaddingNeverReachesTheMessage)
{
        using T = xstd::bit_int<24>;

        auto const clean = T{-5};
        auto clean_bytes = std::array<unsigned char, sizeof(T)>{};
        std::memcpy(clean_bytes.data(), &clean, sizeof(T));

        constexpr auto padding_byte = std::endian::native == std::endian::little ? sizeof(T) - 1UZ : 0UZ;
        auto dirty_bytes            = clean_bytes;
        dirty_bytes.at(padding_byte) ^= 0x5aU;
        auto dirty = T{};
        std::memcpy(&dirty, dirty_bytes.data(), sizeof(T));
        auto stored_bytes = std::array<unsigned char, sizeof(T)>{};
        std::memcpy(stored_bytes.data(), &dirty, sizeof(T));

        // Equal values in unequal objects, which is what hashing the object would tell apart.
        BOOST_CHECK(dirty == clean);
        BOOST_CHECK(stored_bytes != clean_bytes);
        BOOST_CHECK(message_of<boost::hash2::default_flavor>(dirty) == message_of<boost::hash2::default_flavor>(clean));
        BOOST_CHECK(message_of(dirty) == message_of(clean));
        BOOST_CHECK(message_of(dirty) == message_of(std::int32_t{-5}));
}

#endif // XSTD_HAS_BIT_INT

#else

// The adapter needs Boost.Hash2, and Boost.Test wants a case either way.
BOOST_AUTO_TEST_CASE(AbsentWithoutBoostHash2)
{
        BOOST_CHECK(true);
}

#endif // TEST_HAS_BOOST_HASH2

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
