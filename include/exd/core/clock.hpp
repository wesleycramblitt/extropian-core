#pragma once

#include <chrono>
#include <cstdint>

namespace exd::core {

// ────────────────────────────────────────────────────
//  High-resolution clock & frame timer
// ────────────────────────────────────────────────────

/// Monotonic clock for performance measurement.  Wraps std::chrono::steady_clock.
struct Clock {
    using rep     = double;
    using period  = std::ratio<1>;
    using duration = std::chrono::duration<rep, period>;

    /// Seconds since epoch (implementation-defined starting point).
    [[nodiscard]] static rep now_sec() {
        auto t = std::chrono::steady_clock::now().time_since_epoch();
        return std::chrono::duration<rep>(t).count();
    }
};

/// Simple stopwatch.  Call tick() each frame to advance elapsed.
/// elapsed() returns accumulated time in seconds.
struct Stopwatch {
    [[nodiscard]] double elapsed() const noexcept { return elapsed_; }
    [[nodiscard]] double delta()   const noexcept { return delta_; }

    /// Advance the stopwatch by the given delta in seconds.
    void tick(double dt) { delta_ = dt; elapsed_ += dt; }

    void reset() { elapsed_ = 0.0; delta_ = 0.0; }

private:
    double elapsed_ = 0.0;
    double delta_   = 0.0;
};

/// Frame clock: measures wall-clock time between frames.
/// Call mark() at the start of each frame, then call delta().
struct FrameClock {
    FrameClock() : last_(Clock::now_sec()) {}

    /// Mark the end of the previous frame and the start of the next.
    /// Returns the delta time in seconds since the last mark().
    double mark() {
        double now = Clock::now_sec();
        double dt  = now - last_;
        last_ = now;
        return dt;
    }

    [[nodiscard]] double delta() const noexcept { return delta_; }

private:
    double last_  = 0.0;
    double delta_ = 0.0;
};

/// Fixed-timestep accumulator.  Use for physics / simulation loops.
/// Call accumulate(dt) each frame; returns the number of fixed steps to run.
struct FixedTimestep {
    explicit FixedTimestep(double rate_hz) : step_(1.0 / rate_hz) {}

    /// Add frame delta time. Returns how many fixed steps to run this frame.
    [[nodiscard]] int accumulate(double dt) noexcept {
        accumulator_ += dt;
        int steps = 0;
        while (accumulator_ >= step_) {
            accumulator_ -= step_;
            ++steps;
        }
        return steps;
    }

    [[nodiscard]] double step() const noexcept { return step_; }
    [[nodiscard]] double alpha() const noexcept { return accumulator_ / step_; }

    void reset() { accumulator_ = 0.0; }

private:
    double step_        = 0.0;
    double accumulator_ = 0.0;
};

} // namespace exd::core
