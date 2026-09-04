// Structural mutations for the declarative VisualDocument contract.
//
// This is distinct from presentation_state.hpp's runtime PatchDocument: these
// operations mutate authored document structure and address document IDs only.
#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace exd
{

struct VisualDocumentPatchOp
{
    std::string op;              // add_node | remove_node | replace_node | ...
    std::string target;          // stable document node, section, or relation ID
    nlohmann::json params;       // operation-specific options, such as a parent ID
    nlohmann::json value;        // operation-specific authored document value
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VisualDocumentPatchOp, op, target, params, value)

struct VisualDocumentPatch
{
    std::vector<VisualDocumentPatchOp> ops;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VisualDocumentPatch, ops)

} // namespace exd
