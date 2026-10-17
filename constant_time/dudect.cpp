//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cases.hpp>   // cases, inputs, kernel_case
#include <dudect.h>    // dudect_config_t, dudect_free, dudect_init, dudect_main, randombit, randombytes
#include <array>       // array
#include <chrono>      // seconds, steady_clock
#include <cstddef>     // size_t
#include <cstdint>     // uint64_t, uint8_t
#include <cstdio>      // printf
#include <cstdlib>     // strtol
#include <cstring>     // memcpy
#include <string_view> // string_view

namespace {

constant_time::kernel_case const* tested = nullptr;

} // namespace

extern "C" {

// dudect times this once per measurement, on the inputs prepare_inputs laid out.
[[nodiscard]] auto do_one_computation(std::uint8_t* data) -> std::uint8_t
{
        auto in = constant_time::inputs{};
        std::memcpy(&in, data, sizeof(in));
        return static_cast<std::uint8_t>(tested->run(in));
}

// Each measurement's class drawn at random, then its inputs drawn for that class, outside the timed call.
auto prepare_inputs(dudect_config_t* config, std::uint8_t* input_data, std::uint8_t* classes) -> void
{
        for (auto i = std::size_t{0}; i < config->number_measurements; ++i) {
                auto draws = std::array<std::uint64_t, 2>{};
                randombytes(reinterpret_cast<std::uint8_t*>(draws.data()), sizeof(draws));
                classes[i]    = randombit();
                auto const in = classes[i] == 0 ? tested->fixed(draws[0], draws[1]) : tested->random(draws[0], draws[1]);
                std::memcpy(input_data + i * config->chunk_size, &in, sizeof(in));
        }
}

} // extern "C"

// Every case, or the one named, until dudect finds leakage or the budget in seconds per case runs out.
auto main(int argc, char** argv) -> int
{
        auto const budget = std::chrono::seconds{argc > 1 ? std::strtol(argv[1], nullptr, 10) : 60};
        auto const only   = argc > 2 ? std::string_view{argv[2]} : std::string_view{};
        for (auto const& c : constant_time::cases) {
                if (not only.empty() and c.name != only) {
                        continue;
                }
                tested      = &c;
                auto config = dudect_config_t{.chunk_size = sizeof(constant_time::inputs), .number_measurements = 100'000};
                auto ctx    = dudect_ctx_t{};
                dudect_init(&ctx, &config);
                std::printf("## %.*s\n", static_cast<int>(c.name.size()), c.name.data());
                auto const start = std::chrono::steady_clock::now();
                auto state       = DUDECT_NO_LEAKAGE_EVIDENCE_YET;
                while (state == DUDECT_NO_LEAKAGE_EVIDENCE_YET and std::chrono::steady_clock::now() - start < budget) {
                        state = dudect_main(&ctx);
                }
                dudect_free(&ctx);
                std::printf("verdict: %s\n\n", state == DUDECT_LEAKAGE_FOUND ? "leakage found" : "no leakage found within the budget");
                std::fflush(stdout);
        }
        return 0;
}
