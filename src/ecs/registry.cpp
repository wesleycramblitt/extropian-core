#include <ext/ecs/registry.hpp>
#include <cstdio>

namespace ext::ecs {

Entity Registry::create(std::string name) {
    Entity::id_type id;
    if (!free_ids_.empty()) {
        id = free_ids_.back();
        free_ids_.pop_back();
    } else {
        id = static_cast<Entity::id_type>(gen_.size());
        gen_.push_back(0);
        alive_.push_back(0);
        names_.emplace_back();
    }
    alive_[id] = 1;
    names_[id] = std::move(name);
    return {id, gen_[id]};
}

void Registry::destroy(Entity e) {
    if (!valid(e)) return;
    alive_[e.id] = 0;
    gen_[e.id]++;
    free_ids_.push_back(e.id);
    for (auto& [_, pool] : pools_) pool->remove_entity(e.id);
}

bool Registry::valid(Entity e) const noexcept {
    return e.id < gen_.size() && gen_[e.id] == e.gen && alive_[e.id];
}

void Registry::clear() {
    pools_.clear();
    gen_.clear();
    alive_.clear();
    free_ids_.clear();
    names_.clear();
}

std::vector<Entity> Registry::all_entities() const noexcept {
    std::vector<Entity> out;
    for (Entity::id_type id = 0; id < gen_.size(); ++id)
        if (alive_[id])
            out.push_back({id, gen_[id]});
    return out;
}

size_t Registry::entity_count() const noexcept {
    size_t count = 0;
    for (auto a : alive_) if (a) count++;
    return count;
}

} // namespace ext::ecs
