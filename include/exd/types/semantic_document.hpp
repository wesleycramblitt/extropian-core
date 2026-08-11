// Semantic Document (Doc 1) — what concepts exist.
//
// Part of the 4-document pipeline:
//   Doc 1: Semantic Document (entities, relations, processes — what exists)
//   Doc 2: Visual Intent Document (representations, composition — how to explain)
//   Doc 3: Scene Document (spaces, nodes, transforms — the unified contract)
//   Doc 4: Presentation State (runtime focus, annotations, patches)
//
// The AI conductor emits all four documents in a single JSON response.
// This header defines the C++ structs for the Semantic Document layer.
#pragma once

#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace exd
{

// ── Semantic Entity ──

enum class SemanticEntityKind
{
    Point,
    Matrix,
    Vector,
    Scalar,
    Function,
    Process,
    Constraint,
    Concept
};

struct SemanticEntity
{
    std::string id;
    SemanticEntityKind kind;
    std::string label;
    std::string description;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SemanticEntity, id, kind, label, description)

// ── Semantic Process ──

struct SemanticProcess
{
    std::string id;
    std::string type;                           // "transform_sequence", "data_flow", "control_flow"
    std::string input_id;
    std::vector<std::string> operator_ids;
    std::string output_id;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SemanticProcess, id, type, input_id, operator_ids, output_id)

// ── Semantic Relation ──

struct SemanticRelation
{
    std::string id;
    std::string source_id;
    std::string target_id;
    std::string kind;                           // "depends_on", "transforms_into", "causes", "contains"
    std::string label;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SemanticRelation, id, source_id, target_id, kind, label)

// ── Top-Level Semantic Document ──

struct SemanticDocument
{
    std::string topic;
    std::string goal;
    std::vector<SemanticEntity> entities;
    std::vector<SemanticProcess> processes;
    std::vector<SemanticRelation> relations;
    std::map<std::string, double> quantities;
    std::vector<std::string> interactions;      // "edit_matrices", "move_vertex", etc.
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SemanticDocument, topic, goal, entities, processes, relations, quantities, interactions)

} // namespace exd
