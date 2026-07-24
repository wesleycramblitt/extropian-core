#pragma once

#include <cstdint>
#include <limits>
#include <random>
#include <concepts>

namespace exd::core {

// ────────────────────────────────────────────────────
//  PCG32 fast seeded random number generator
//  (Permuted Congruential Generator, minimal C++ impl)
// ────────────────────────────────────────────────────

/// 32-bit PCG random number generator.
/// Small state (8 bytes), good statistical quality, fast.
class RNG {
public:
    using result_type = uint32_t;

    explicit RNG(uint64_t seed = 0x853c49e6748fea9bULL) : state_(seed) {
        step(); // advance once to avoid zero-state bias
    }

    /// Seed the generator (replaces state entirely).
    void seed(uint64_t s) { state_ = s + inc_; step(); }

    /// Generate a uniform random uint32_t.
    uint32_t next_u32() { return step(); }

    /// Uniform float in [0, 1).
    float next_f32() {
        return static_cast<float>(next_u32())
             / (static_cast<float>(std::numeric_limits<uint32_t>::max()) + 1.0f);
    }

    /// Uniform float in [min, max).
    float next_f32_range(float min, float max) {
        return min + (max - min) * next_f32();
    }

    /// Uniform int in [min, max] (inclusive).
    int next_int(int min, int max) {
        return min + static_cast<int>(next_u32()
              % static_cast<uint32_t>(max - min + 1));
    }

    /// Uniform double in [0, 1).
    double next_f64() {
        return static_cast<double>(next_u32())
             / (static_cast<double>(std::numeric_limits<uint32_t>::max()) + 1.0);
    }

    /// Bernoulli trial (true with probability p).
    bool chance(float p) { return next_f32() < p; }

    /// Fill a trivially-copyable value with random bytes.
    template <typename T>
    requires std::is_trivially_copyable_v<T>
    void fill(T& val) {
        auto* p = reinterpret_cast<uint8_t*>(&val);
        for (size_t i = 0; i < sizeof(T); i += 4) {
            uint32_t r = next_u32();
            for (size_t j = 0; j < 4 && (i + j) < sizeof(T); ++j)
                p[i + j] = static_cast<uint8_t>(r >> (j * 8));
        }
    }

private:
    uint64_t state_ = 0;
    uint64_t inc_   = 1442695040888963407ULL;

    uint32_t step() {
        uint64_t old = state_;
        state_ = old * 6364136223846793005ULL + (inc_ | 1);
        uint32_t xorshifted = static_cast<uint32_t>(((old >> 18u) ^ old) >> 27u);
        uint32_t rot = static_cast<uint32_t>(old >> 59u);
        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }
};

/// Standard Mersenne Twister wrapper (from <random>).
/// Use for high-quality non-deterministic distributions.
struct Random {
    std::mt19937_64 mt;

    Random() {
        std::random_device rd;
        mt.seed(rd());
    }
    explicit Random(uint64_t seed) : mt(seed) {}

    [[nodiscard]] float uniform(float min = 0.0f, float max = 1.0f) {
        return std::uniform_real_distribution<float>(min, max)(mt);
    }
    [[nodiscard]] int uniform_int(int min, int max) {
        return std::uniform_int_distribution<int>(min, max)(mt);
    }
    [[nodiscard]] float normal(float mean = 0.0f, float stddev = 1.0f) {
        return std::normal_distribution<float>(mean, stddev)(mt);
    }
    [[nodiscard]] bool chance(float p = 0.5f) {
        return std::bernoulli_distribution(p)(mt);
    }

    void seed(uint64_t s) { mt.seed(s); }
};

} // namespace exd::core
