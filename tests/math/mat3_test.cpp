#include <doctest/doctest.h>
#include <exd/math/mat3.hpp>

using namespace exd::math;

TEST_CASE("Mat3 identity") {
    auto I = Mat3::identity();
    CHECK(I.m[0] == 1.0f); CHECK(I.m[1] == 0.0f); CHECK(I.m[2] == 0.0f);
    CHECK(I.m[3] == 0.0f); CHECK(I.m[4] == 1.0f); CHECK(I.m[5] == 0.0f);
    CHECK(I.m[6] == 0.0f); CHECK(I.m[7] == 0.0f); CHECK(I.m[8] == 1.0f);
}

TEST_CASE("Mat3 zero") {
    auto Z = Mat3::zero();
    for (int i = 0; i < 9; ++i) CHECK(Z.m[i] == 0.0f);
}

TEST_CASE("Mat3 multiply by identity") {
    auto I = Mat3::identity();
    Mat3 M{};
    // Fill with some values
    M.m[0]=1; M.m[1]=2; M.m[2]=3;
    M.m[3]=4; M.m[4]=5; M.m[5]=6;
    M.m[6]=7; M.m[7]=8; M.m[8]=9;

    auto R = Mat3::mul(M, I);
    for (int i = 0; i < 9; ++i) CHECK(R.m[i] == M.m[i]);

    R = Mat3::mul(I, M);
    for (int i = 0; i < 9; ++i) CHECK(R.m[i] == M.m[i]);
}

TEST_CASE("Mat3 multiply") {
    // Two known matrices in column-major
    // A = [1 4 7; 2 5 8; 3 6 9]  (column-major: m[0]=1,m[1]=2,m[2]=3, m[3]=4,m[4]=5,m[5]=6, m[6]=7,m[7]=8,m[8]=9)
    Mat3 A{}; A.m[0]=1; A.m[1]=2; A.m[2]=3;  A.m[3]=4; A.m[4]=5; A.m[5]=6;  A.m[6]=7; A.m[7]=8; A.m[8]=9;
    // B = [9 6 3; 8 5 2; 7 4 1]  (column-major: m[0]=9,m[1]=8,m[2]=7, m[3]=6,m[4]=5,m[5]=4, m[6]=3,m[7]=2,m[8]=1)
    Mat3 B{}; B.m[0]=9; B.m[1]=8; B.m[2]=7;  B.m[3]=6; B.m[4]=5; B.m[5]=4;  B.m[6]=3; B.m[7]=2; B.m[8]=1;

    auto C = Mat3::mul(A, B);
    // A*B in column-major: c[0] = A[0]*B[0] + A[3]*B[1] + A[6]*B[2] = 1*9 + 4*8 + 7*7 = 9+32+49 = 90
    CHECK(C.m[0] == doctest::Approx(90.0f));
    // c[4] = A[1]*B[3] + A[4]*B[4] + A[7]*B[5] = 2*6 + 5*5 + 8*4 = 12+25+32 = 69
    CHECK(C.m[4] == doctest::Approx(69.0f));
    // c[8] = A[2]*B[6] + A[5]*B[7] + A[8]*B[8] = 3*3 + 6*2 + 9*1 = 9+12+9 = 30
    CHECK(C.m[8] == doctest::Approx(30.0f));
}

TEST_CASE("Mat3 transpose") {
    Mat3 M{}; M.m[0]=1; M.m[1]=2; M.m[2]=3; M.m[3]=4; M.m[4]=5; M.m[5]=6; M.m[6]=7; M.m[7]=8; M.m[8]=9;
    auto T = Mat3::transpose(M);
    CHECK(T.m[0]==1); CHECK(T.m[1]==4); CHECK(T.m[2]==7);
    CHECK(T.m[3]==2); CHECK(T.m[4]==5); CHECK(T.m[5]==8);
    CHECK(T.m[6]==3); CHECK(T.m[7]==6); CHECK(T.m[8]==9);

    // Double transpose = original
    auto TT = Mat3::transpose(T);
    for (int i = 0; i < 9; ++i) CHECK(TT.m[i] == M.m[i]);
}

TEST_CASE("Mat3 determinant") {
    auto I = Mat3::identity();
    CHECK(Mat3::det(I) == doctest::Approx(1.0f));

    // Scaled: det should be 2*3*4 = 24
    Mat3 S{}; S.m[0]=2; S.m[4]=3; S.m[8]=4;
    CHECK(Mat3::det(S) == doctest::Approx(24.0f));

    // Singular matrix (two identical rows)
    Mat3 singular{}; singular.m[0]=1; singular.m[1]=1; singular.m[2]=1;
                     singular.m[3]=2; singular.m[4]=2; singular.m[5]=2;
                     singular.m[6]=3; singular.m[7]=3; singular.m[8]=3;
    CHECK(Mat3::det(singular) == doctest::Approx(0.0f));
}

TEST_CASE("Mat3 inverse") {
    auto I = Mat3::identity();
    auto invI = Mat3::inverse(I);
    for (int i = 0; i < 9; ++i) CHECK(invI.m[i] == doctest::Approx(I.m[i]));

    // M * M^-1 = I
    Mat3 M{}; M.m[0]=3; M.m[3]=0; M.m[6]=2;
              M.m[1]=2; M.m[4]=0; M.m[7]=-2;
              M.m[2]=0; M.m[5]=1; M.m[8]=1;
    auto inv = Mat3::inverse(M);
    auto prod = Mat3::mul(M, inv);
    CHECK(prod.m[0] == doctest::Approx(1.0f));
    CHECK(prod.m[4] == doctest::Approx(1.0f));
    CHECK(prod.m[8] == doctest::Approx(1.0f));
    CHECK(prod.m[1] == doctest::Approx(0.0f));
    CHECK(prod.m[2] == doctest::Approx(0.0f));
    CHECK(prod.m[3] == doctest::Approx(0.0f));
}

TEST_CASE("Mat3 from_axis_angle") {
    // 90 degrees around Z axis
    auto R = Mat3::from_axis_angle(Vec3f{0, 0, 1}, M_PI / 2.0f);

    // Rotating (1,0,0) around Z by 90° should give (0,1,0)
    auto v = Mat3::mul_vec(R, Vec3f{1, 0, 0});
    CHECK(v.x == doctest::Approx(0.0f).epsilon(1e-5f));
    CHECK(v.y == doctest::Approx(1.0f).epsilon(1e-5f));
    CHECK(v.z == doctest::Approx(0.0f).epsilon(1e-5f));

    // Rotating (0,1,0) around Z by 90° should give (-1,0,0)
    auto v2 = Mat3::mul_vec(R, Vec3f{0, 1, 0});
    CHECK(v2.x == doctest::Approx(-1.0f).epsilon(1e-5f));
    CHECK(v2.y == doctest::Approx(0.0f).epsilon(1e-5f));
    CHECK(v2.z == doctest::Approx(0.0f).epsilon(1e-5f));
}

TEST_CASE("Mat3 from_scale") {
    auto S = Mat3::from_scale(Vec3f{2, 3, 4});
    CHECK(S.m[0] == 2.0f); CHECK(S.m[4] == 3.0f); CHECK(S.m[8] == 4.0f);
    CHECK(S.m[1] == 0.0f); CHECK(S.m[2] == 0.0f); CHECK(S.m[3] == 0.0f);
}

TEST_CASE("Mat3 from_mat4") {
    // Simulate a Mat4 upper-left 3x3
    float m4[16] = {
        1, 2, 3, 0,
        4, 5, 6, 0,
        7, 8, 9, 0,
        10,11,12,1
    };
    auto M3 = Mat3::from_mat4(m4);
    CHECK(M3.m[0] == 1.0f); CHECK(M3.m[1] == 2.0f); CHECK(M3.m[2] == 3.0f);
    CHECK(M3.m[3] == 4.0f); CHECK(M3.m[4] == 5.0f); CHECK(M3.m[5] == 6.0f);
    CHECK(M3.m[6] == 7.0f); CHECK(M3.m[7] == 8.0f); CHECK(M3.m[8] == 9.0f);
}

TEST_CASE("Mat3 mul_vec") {
    // Identity
    auto I = Mat3::identity();
    Vec3f v{1, 2, 3};
    CHECK(Mat3::mul_vec(I, v) == v);

    // Scale matrix
    auto S = Mat3::from_scale(Vec3f{2, 3, 4});
    auto sv = Mat3::mul_vec(S, v);
    CHECK(sv == Vec3f{2, 6, 12});
}
