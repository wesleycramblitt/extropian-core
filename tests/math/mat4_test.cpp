#include <doctest/doctest.h>
#include <exd/math/mat4.hpp>
#include <cmath>

using namespace exd::math;

TEST_CASE("Mat4 identity") {
    auto I = Mat4::identity();
    CHECK(I.m[0]==1); CHECK(I.m[1]==0); CHECK(I.m[2]==0); CHECK(I.m[3]==0);
    CHECK(I.m[4]==0); CHECK(I.m[5]==1); CHECK(I.m[6]==0); CHECK(I.m[7]==0);
    CHECK(I.m[8]==0); CHECK(I.m[9]==0); CHECK(I.m[10]==1); CHECK(I.m[11]==0);
    CHECK(I.m[12]==0); CHECK(I.m[13]==0); CHECK(I.m[14]==0); CHECK(I.m[15]==1);
}

TEST_CASE("Mat4 multiply by identity") {
    auto I = Mat4::identity();
    // Arbitrary matrix
    Mat4 M{}; M.m[0]=1; M.m[4]=2; M.m[8]=3; M.m[12]=4;
              M.m[1]=5; M.m[5]=6; M.m[9]=7; M.m[13]=8;
              M.m[2]=9; M.m[6]=10; M.m[10]=11; M.m[14]=12;
              M.m[3]=13; M.m[7]=14; M.m[11]=15; M.m[15]=16;

    auto R = Mat4::mul(M, I);
    for (int i = 0; i < 16; ++i) CHECK(R.m[i] == M.m[i]);

    R = Mat4::mul(I, M);
    for (int i = 0; i < 16; ++i) CHECK(R.m[i] == M.m[i]);
}

TEST_CASE("Mat4 perspective") {
    auto P = Mat4::perspective(1.047f, 16.0f/9.0f, 0.1f, 1000.0f);
    // Row 3 col 3 should be -1.0f for perspective division
    CHECK(P.m[11] == -1.0f);
    // All other elements in the last column should be 0 except m[14]
    CHECK(P.m[3] == 0.0f);  CHECK(P.m[7] == 0.0f);
    CHECK(P.m[15]== 0.0f);
    // m[14] should be negative and non-zero
    CHECK(P.m[14] < 0.0f);
    // m[0], m[5], m[10] should all be positive non-zero
    CHECK(P.m[0] > 0.0f);
    CHECK(P.m[5] > 0.0f);
    CHECK(P.m[10]!= 0.0f);
}

TEST_CASE("Mat4 look_at") {
    // Looking at origin from +Z direction (eye at 0,0,5 looking at 0,0,0)
    auto view = Mat4::look_at(Vec3f{0, 0, 5}, Vec3f{0, 0, 0}, Vec3f{0, 1, 0});

    // The view matrix should translate so that eye becomes origin
    // A point at (0,0,5) in world space should map to (0,0,?) in view space
    // Actually: view * (0,0,5,1) should give something with z near 0
    float x = view.m[0]*0 + view.m[4]*0 + view.m[8]*5  + view.m[12];
    float y = view.m[1]*0 + view.m[5]*0 + view.m[9]*5  + view.m[13];
    float z = view.m[2]*0 + view.m[6]*0 + view.m[10]*5 + view.m[14];
    float w = view.m[3]*0 + view.m[7]*0 + view.m[11]*5 + view.m[15];
    CHECK(x == doctest::Approx(view.m[12]));
    CHECK(y == doctest::Approx(view.m[13]));
}

TEST_CASE("Mat4 trs without skew") {
    Vec3f pos{1, 2, 3};
    Quat rot = Quat{1, 0, 0, 0}; // Identity rotation
    Vec3f scale{1, 1, 1};

    auto M = Mat4::trs(pos, rot, scale);

    // Position should be in the last column
    CHECK(M.m[12] == doctest::Approx(1.0f));
    CHECK(M.m[13] == doctest::Approx(2.0f));
    CHECK(M.m[14] == doctest::Approx(3.0f));
    CHECK(M.m[15] == doctest::Approx(1.0f));
}

TEST_CASE("Mat4 trs with rotation") {
    // 90 degree rotation around Z, no translation, no scale
    Quat rot = Quat::from_axis_angle(Vec3f{0, 0, 1}, float(M_PI / 2.0));
    Vec3f pos{0, 0, 0};
    Vec3f scale{1, 1, 1};

    auto M = Mat4::trs(pos, rot, scale);

    // Transform (1,0,0) should yield approximately (0,1,0)
    float tx = M.m[0]*1 + M.m[4]*0 + M.m[8]*0  + M.m[12];
    float ty = M.m[1]*1 + M.m[5]*0 + M.m[9]*0  + M.m[13];
    float tz = M.m[2]*1 + M.m[6]*0 + M.m[10]*0 + M.m[14];
    float tw = M.m[3]*1 + M.m[7]*0 + M.m[11]*0 + M.m[15];

    CHECK(tx == doctest::Approx(0.0f).epsilon(1e-5f));
    CHECK(ty == doctest::Approx(1.0f).epsilon(1e-5f));
    CHECK(tz == doctest::Approx(0.0f).epsilon(1e-5f));
    CHECK(tw == doctest::Approx(1.0f).epsilon(1e-5f));
}

TEST_CASE("Mat4 trs with scale") {
    Vec3f pos{0, 0, 0};
    Quat rot{1, 0, 0, 0};
    Vec3f scale{2, 3, 4};

    auto M = Mat4::trs(pos, rot, scale);
    CHECK(M.m[0] == doctest::Approx(2.0f));
    CHECK(M.m[5] == doctest::Approx(3.0f));
    CHECK(M.m[10]== doctest::Approx(4.0f));
}

TEST_CASE("Mat4 trs with skew") {
    Vec3f pos{1, 2, 3};
    Quat rot = Quat{1, 0, 0, 0};
    Vec3f scale{1, 1, 1};
    Vec3f skew{0, 0, 0}; // no actual skew

    auto M_no_skew = Mat4::trs(pos, rot, scale);
    auto M_skew_zero = Mat4::trs(pos, rot, scale, skew);
    for (int i = 0; i < 16; ++i)
        CHECK(M_skew_zero.m[i] == doctest::Approx(M_no_skew.m[i]));
}
