// Nlohmann JSON support for std::optional<T>
//
// nlohmann::json v3.11.x does not provide built-in adl_serializer for
// std::optional<T>. The NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macro generates
// code that calls json::get_to() for deserialization and json::operator=()
// for serialization. Both need std::optional support via free from_json/to_json
// functions found by ADL.
//
// Include this before any NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE that uses
// std::optional fields.
#pragma once

#include <optional>
#include <nlohmann/json.hpp>

namespace nlohmann
{

// ── from_json: deserialize JSON to std::optional<T> ──
//
// Called by json::get_to(optional<T>&) internally.
// Enables NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macros to read
// std::optional fields from JSON.

template <typename T>
void from_json(const json& j, std::optional<T>& opt)
{
    if (j.is_null())
        opt = std::nullopt;
    else
        opt = j.get<T>();
}

// ── to_json: serialize std::optional<T> to JSON ──
//
// Called by json::operator=(const optional<T>&) via ADL.
// Enables NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macros to write
// std::optional fields to JSON.

template <typename T>
void to_json(json& j, const std::optional<T>& opt)
{
    if (opt.has_value())
        j = opt.value();
    else
        j = nullptr;
}

} // namespace nlohmann
