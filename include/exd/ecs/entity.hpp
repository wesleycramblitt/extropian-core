#pragma once

#include <cstdint>
#include <string>
#include <limits>

namespace exd::ecs {

// ────────────────────────────────────────────────────
//  Entity — lightweight handle with generation counter
// ────────────────────────────────────────────────────
//  Generation counter prevents dangling references
//  after an entity is destroyed and its ID recycled.

struct Entity {
    using id_type  = uint32_t;
    using gen_type = uint32_t;

    id_type  id{std::numeric_limits<id_type>::max()};
    gen_type gen{0};
    std::string name;

    friend bool operator==(const Entity& a, const Entity& b) noexcept {
        return a.id == b.id && a.gen == b.gen;
    }
    friend bool operator!=(const Entity& a, const Entity& b) noexcept {
        return !(a == b);
    }
};

} // namespace exd::ecs
