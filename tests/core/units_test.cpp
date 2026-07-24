#include <doctest/doctest.h>
#include <exd/core/units.hpp>
#include <cmath>

using namespace exd::core::units;

TEST_CASE("radians_to_degrees") {
    CHECK(radians_to_degrees(0.0) == doctest::Approx(0.0));
    CHECK(radians_to_degrees(M_PI) == doctest::Approx(180.0));
    CHECK(radians_to_degrees(M_PI / 2.0) == doctest::Approx(90.0));
    CHECK(radians_to_degrees(2.0 * M_PI) == doctest::Approx(360.0));
}

TEST_CASE("degrees_to_radians") {
    CHECK(degrees_to_radians(0.0) == doctest::Approx(0.0));
    CHECK(degrees_to_radians(180.0) == doctest::Approx(M_PI));
    CHECK(degrees_to_radians(90.0) == doctest::Approx(M_PI / 2.0));
    CHECK(degrees_to_radians(360.0) == doctest::Approx(2.0 * M_PI));
}

TEST_CASE("radians_to_degrees round-trip") {
    double rad = 1.234;
    double deg = radians_to_degrees(rad);
    double back = degrees_to_radians(deg);
    CHECK(back == doctest::Approx(rad));
}

TEST_CASE("f_from_u_rectangular_duct basic") {
    // Typical values for a duct
    double width = 0.1, height = 0.05;
    double rho = 1.2, nu = 1.5e-5;
    double target_u = 0.01;

    double f = f_from_u_rectangular_duct(width, height, rho, nu, target_u);
    // Should produce a non-negative finite result
    CHECK(f >= 0.0);
    CHECK(std::isfinite(f));
}

TEST_CASE("f_from_u_rectangular_duct zero velocity") {
    double f = f_from_u_rectangular_duct(0.1, 0.05, 1.2, 1.5e-5, 0.0);
    CHECK(f == doctest::Approx(0.0));  // early-exit guard
}

TEST_CASE("f_from_u_rectangular_duct square duct") {
    double f_square = f_from_u_rectangular_duct(0.1, 0.1, 1.2, 1.5e-5, 0.01);
    CHECK(std::isfinite(f_square));
}
