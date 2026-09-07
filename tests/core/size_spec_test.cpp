#include <doctest/doctest.h>

#include <exd/types/size_spec.hpp>

using exd::SizeSpec;
using exd::SizeSpecKind;

TEST_CASE("parse_size_spec accepts the size vocabulary")
{
    SUBCASE("auto")
    {
        auto a = exd::parse_size_spec("");
        REQUIRE(a.has_value());
        CHECK(a->kind == SizeSpecKind::Auto);

        auto b = exd::parse_size_spec("auto");
        REQUIRE(b.has_value());
        CHECK(b->kind == SizeSpecKind::Auto);
    }

    SUBCASE("fill")
    {
        auto s = exd::parse_size_spec("fill");
        REQUIRE(s.has_value());
        CHECK(s->kind == SizeSpecKind::Fill);
    }

    SUBCASE("px")
    {
        auto s = exd::parse_size_spec("12");
        REQUIRE(s.has_value());
        CHECK(s->kind == SizeSpecKind::Px);
        CHECK(s->value == doctest::Approx(12.0f));
    }

    SUBCASE("decimal px")
    {
        auto s = exd::parse_size_spec("8.5");
        REQUIRE(s.has_value());
        CHECK(s->kind == SizeSpecKind::Px);
        CHECK(s->value == doctest::Approx(8.5f));
    }

    SUBCASE("whitespace")
    {
        auto s = exd::parse_size_spec(" 40 ");
        REQUIRE(s.has_value());
        CHECK(s->kind == SizeSpecKind::Px);
        CHECK(s->value == doctest::Approx(40.0f));
    }

    SUBCASE("percent")
    {
        auto s = exd::parse_size_spec("12%");
        REQUIRE(s.has_value());
        CHECK(s->kind == SizeSpecKind::Percent);
        CHECK(s->value == doctest::Approx(12.0f));
    }

    SUBCASE("rem")
    {
        auto s = exd::parse_size_spec("0.5rem");
        REQUIRE(s.has_value());
        CHECK(s->kind == SizeSpecKind::Rem);
        CHECK(s->value == doctest::Approx(0.5f));
    }

    SUBCASE("decimal rem")
    {
        auto s = exd::parse_size_spec("1.25rem");
        REQUIRE(s.has_value());
        CHECK(s->kind == SizeSpecKind::Rem);
        CHECK(s->value == doctest::Approx(1.25f));
    }
}

TEST_CASE("parse_size_spec rejects invalid text")
{
    CHECK_FALSE(exd::parse_size_spec("8/12").has_value());
    CHECK_FALSE(exd::parse_size_spec("12px").has_value());
    CHECK_FALSE(exd::parse_size_spec("-3").has_value());
    CHECK_FALSE(exd::parse_size_spec("abc").has_value());
    CHECK_FALSE(exd::parse_size_spec("%").has_value());
}
