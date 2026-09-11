#include <exd/ecs/transform_system.hpp>

#include <exd/ecs/view.hpp>

#include <unordered_set>

namespace exd::ecs {

WorldTransform TransformSystem::derive(const Registry& registry,
                                       const SceneGraph& scene,
                                       Entity entity) {
    WorldTransform result;  // identity
    const LocalTransform* local = registry.try_get<LocalTransform>(entity);
    if (!local) return result;

    result.position = local->position;
    result.rotation = local->rotation;
    result.scale = local->scale;

    // Guard against malformed/stale cycles (attach() prevents cycles, but
    // pruned nodes or user error could still produce one).
    std::unordered_set<Entity, SceneGraph::EntityHash> visited;
    Entity current = entity;
    while (visited.insert(current).second) {
        const auto parent = scene.parent(current);
        if (!parent) break;
        current = *parent;
        const LocalTransform* p = registry.try_get<LocalTransform>(current);
        if (!p) continue;  // parent contributes identity
        // world = parent_world * local, decomposed without matrices
        // (component-wise scale accumulation, standard TRS composition):
        const math::Vec3f scaled = {p->scale.x * result.position.x,
                                    p->scale.y * result.position.y,
                                    p->scale.z * result.position.z};
        result.position = p->position + p->rotation * scaled;
        result.rotation = p->rotation * result.rotation;
        result.scale = {p->scale.x * result.scale.x,
                        p->scale.y * result.scale.y,
                        p->scale.z * result.scale.z};
    }
    return result;
}

void TransformSystem::update(Registry& registry, double /*dt*/) {
    for (auto e : registry.view<LocalTransform>()) {
        registry.emplace<WorldTransform>(e, derive(registry, scene_, e));
    }
}

} // namespace exd::ecs
