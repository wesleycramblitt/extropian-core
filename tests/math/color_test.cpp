#include <doctest/doctest.h>
#include <exd/math/color.hpp>

using namespace exd::math;

TEST_CASE("ColorRGB defaults to black") {
    ColorRGB c;
    CHECK(c.r == 0.0f); CHECK(c.g == 0.0f); CHECK(c.b == 0.0f);
}

TEST_CASE("ColorRGBA defaults to transparent black") {
    ColorRGBA c;
    CHECK(c.r == 0.0f); CHECK(c.g == 0.0f);
    CHECK(c.b == 0.0f); CHECK(c.a == 1.0f);
}

TEST_CASE("ColorRGB scalar multiply") {
    ColorRGB c{0.5f, 0.25f, 0.1f};
    auto c2 = c * 2.0f;
    CHECK(c2.r == doctest::Approx(1.0f));
    CHECK(c2.g == doctest::Approx(0.5f));
    CHECK(c2.b == doctest::Approx(0.2f));
}

TEST_CASE("ColorRGB add") {
    ColorRGB a{0.2f, 0.3f, 0.4f};
    ColorRGB b{0.1f, 0.1f, 0.1f};
    auto c = a + b;
    CHECK(c.r == doctest::Approx(0.3f));
    CHECK(c.g == doctest::Approx(0.4f));
    CHECK(c.b == doctest::Approx(0.5f));
}

TEST_CASE("ColorRGB modulate") {
    ColorRGB a{1.0f, 0.5f, 0.2f};
    ColorRGB b{0.5f, 0.5f, 0.5f};
    auto c = a * b;
    CHECK(c.r == doctest::Approx(0.5f));
    CHECK(c.g == doctest::Approx(0.25f));
    CHECK(c.b == doctest::Approx(0.1f));
}

TEST_CASE("ColorRGB saturate") {
    ColorRGB c{1.5f, -0.5f, 0.5f};
    auto s = c.saturate();
    CHECK(s.r == doctest::Approx(1.0f));
    CHECK(s.g == doctest::Approx(0.0f));
    CHECK(s.b == doctest::Approx(0.5f));
}

TEST_CASE("ColorRGB srgb_to_linear") {
    // Known values: sRGB 0.5 -> linear ~0.214
    ColorRGB srgb{0.5f, 0.5f, 0.5f};
    auto lin = srgb.srgb_to_linear();
    CHECK(lin.r == doctest::Approx(0.214f).epsilon(0.01f));
}

TEST_CASE("ColorRGB linear_to_srgb") {
    // Round-trip: linear -> srgb -> linear
    ColorRGB lin{0.214f, 0.214f, 0.214f};
    auto srgb = lin.linear_to_srgb();
    auto lin2 = srgb.srgb_to_linear();
    CHECK(lin2.r == doctest::Approx(lin.r).epsilon(0.02f));
}

TEST_CASE("ColorRGB static colors") {
    CHECK(ColorRGB::black()   == ColorRGB{0, 0, 0});
    CHECK(ColorRGB::white()   == ColorRGB{1, 1, 1});
    CHECK(ColorRGB::red()     == ColorRGB{1, 0, 0});
    CHECK(ColorRGB::green()   == ColorRGB{0, 1, 0});
    CHECK(ColorRGB::blue()    == ColorRGB{0, 0, 1});
}

TEST_CASE("ColorRGBA from_u8") {
    auto c = ColorRGBA::from_u8(255, 128, 0, 64);
    CHECK(c.r == doctest::Approx(1.0f));
    CHECK(c.g == doctest::Approx(0.502f).epsilon(0.01f));
    CHECK(c.b == doctest::Approx(0.0f));
    CHECK(c.a == doctest::Approx(0.251f).epsilon(0.01f));
}

TEST_CASE("ColorRGBA from_hex and to_hex") {
    auto c = ColorRGBA::from_hex(0xFF8000FF);
    auto hex = c.to_hex();
    CHECK(hex == 0xFF8000FF);
}

TEST_CASE("ColorRGBA rgb extraction") {
    ColorRGBA c{0.1f, 0.2f, 0.3f, 0.5f};
    auto rgb = c.rgb();
    CHECK(rgb.r == doctest::Approx(0.1f));
    CHECK(rgb.g == doctest::Approx(0.2f));
    CHECK(rgb.b == doctest::Approx(0.3f));
}

TEST_CASE("ColorRGBA static colors") {
    CHECK(ColorRGBA::black()       == ColorRGBA{0, 0, 0, 1});
    CHECK(ColorRGBA::transparent() == ColorRGBA{0, 0, 0, 0});
}
