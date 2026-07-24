#include <doctest/doctest.h>
#include <exd/core/clock.hpp>
#include <thread>

using namespace exd::core;

TEST_CASE("Clock now_sec is monotonic") {
    double t1 = Clock::now_sec();
    // Busy-wait a tiny bit
    for (int i = 0; i < 100000; ++i) __asm__ volatile("");
    double t2 = Clock::now_sec();
    CHECK(t2 >= t1);
}

TEST_CASE("Stopwatch tick and reset") {
    Stopwatch sw;
    CHECK(sw.elapsed() == doctest::Approx(0.0));
    sw.tick(0.016);
    CHECK(sw.delta() == doctest::Approx(0.016));
    CHECK(sw.elapsed() == doctest::Approx(0.016));
    sw.tick(0.016);
    CHECK(sw.elapsed() == doctest::Approx(0.032));
    sw.reset();
    CHECK(sw.elapsed() == doctest::Approx(0.0));
}

TEST_CASE("FrameClock mark") {
    FrameClock fc;
    double dt = fc.mark();
    // First mark returns time since construction, should be tiny
    CHECK(dt >= 0.0);
    CHECK(dt < 0.1); // should be near-instant
}

TEST_CASE("FixedTimestep at 60 Hz") {
    FixedTimestep ft(60.0);
    CHECK(ft.step() == doctest::Approx(1.0 / 60.0));

    // 1 second of frame time should yield ~60 steps (floating-point may give 59)
    int steps = ft.accumulate(1.0);
    CHECK(steps >= 59);
    CHECK(steps <= 60);
}

TEST_CASE("FixedTimestep partial accumulation") {
    FixedTimestep ft(60.0);
    int steps = ft.accumulate(0.03); // slightly more than 1/60
    CHECK(steps == 1); // one step consumed
    double alpha = ft.alpha();
    CHECK(alpha > 0.0);
    CHECK(alpha < 1.0);
}

TEST_CASE("FixedTimestep reset") {
    FixedTimestep ft(10.0);
    (void)ft.accumulate(0.5);
    ft.reset();
    int steps = ft.accumulate(0.01);
    CHECK(steps == 0);
}
