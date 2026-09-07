#include <doctest/doctest.h>

#include <exd/ext.hpp>

TEST_CASE("VisualDocument serializes the declarative 2D contract")
{
    exd::VisualDocument document;
    document.id = "doc-1";
    document.metadata.title = "Quarterly results";
    document.canvas.width = 1200.0f;
    document.canvas.aspect_ratio = "16:9";
    document.density = "dense";
    document.composition = "dashboard";
    document.style = exd::VisualStyleIntent{"primary", 0.95f, true};

    exd::VisualNode chart;
    chart.id = "revenue";
    chart.kind = exd::VisualNodeKind::Chart;
    chart.semantic = exd::VisualSemanticMetadata{"visualization", "revenue", "scalar",
        "Monthly revenue", {"finance", "metric"}};
    chart.style = exd::VisualStyleIntent{"prominent", 0.9f, true};
    chart.interaction = exd::VisualInteraction{false, true, true, true, false, false, true, true,
        {"inspect"}, true, true, false};
    chart.ports = {exd::VisualPort{"input", "west", 0.5f}, exd::VisualPort{"output", "east", 0.5f}};
    chart.shape = exd::VisualShapeSpec{exd::VisualShapeKind::RoundedRectangle, 4.0f, std::nullopt, {}};
    chart.geometry = exd::VisualGeometrySpec{320.0f, 180.0f, std::nullopt, chart.shape, {}};
    chart.binding = exd::VisualDataBinding{"sales", "rows", std::nullopt};
    chart.chart = exd::ChartSpec{
        "line",
        exd::ChartAxis{"Month", "time", "month"},
        exd::ChartAxis{"Revenue", "linear", "revenue"},
        {exd::ChartSeries{"revenue-series", "revenue", std::string{"Revenue"}, std::nullopt}},
        true,
        std::nullopt};
    chart.layout = exd::VisualLayout{"grid", std::nullopt, std::nullopt, std::string{"8/12"},
        std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::nullopt, std::nullopt, {}, {{"columns", 2}}};

    exd::VisualSection section;
    section.id = "summary";
    section.nodes.push_back(chart);
    document.sections.push_back(section);
    document.relations.push_back(exd::VisualRelation{"revenue-link", "revenue", "summary", "output",
        "input", "controls", std::nullopt, exd::VisualRelationStyle{"arrow", std::nullopt, 2.0f, false}, {}});
    document.initial_state = {{"period", "Q2"}};

    const nlohmann::json json = document;
    CHECK(json["version"] == "1");
    CHECK(json["sections"][0]["nodes"][0]["kind"] == "chart");
    CHECK(json["sections"][0]["nodes"][0]["chart"]["series"][0]["id"] == "revenue-series");
    CHECK(json["initial_state"]["period"] == "Q2");
    CHECK(json["density"] == "dense");
    CHECK(json["composition"] == "dashboard");
    CHECK(json["style"]["emphasis"] == "primary");
    CHECK(json["sections"][0]["nodes"][0]["semantic"]["concept_id"] == "revenue");
    CHECK(json["sections"][0]["nodes"][0]["shape"]["kind"] == "rounded_rectangle");
    CHECK(json["sections"][0]["nodes"][0]["geometry"]["width"] == 320.0f);
    CHECK(json["sections"][0]["nodes"][0]["ports"][0]["id"] == "input");
    CHECK(json["sections"][0]["nodes"][0]["interaction"]["activate"] == true);
    CHECK(json["sections"][0]["nodes"][0]["layout"]["params"]["columns"] == 2);
    CHECK(json["relations"][0]["source_port"] == "output");
    CHECK(json["relations"][0]["style"]["type"] == "arrow");
    CHECK_FALSE(json["sections"][0]["nodes"][0].contains("x"));
    CHECK_FALSE(json["sections"][0]["nodes"][0].contains("transform"));
    CHECK_FALSE(json["sections"][0]["nodes"][0].contains("camera"));

    const auto round_trip = json.get<exd::VisualDocument>();
    CHECK(round_trip.composition == document.composition);
    CHECK(round_trip.sections[0].nodes[0].semantic->concept_id == "revenue");
    CHECK(round_trip.sections[0].nodes[0].interaction->focus);
    CHECK(round_trip.sections[0].nodes[0].ports[1].side == "east");
    CHECK(round_trip.relations[0].style->width == doctest::Approx(2.0f));
}

TEST_CASE("VisualDocument round-trips optional fields and hierarchy")
{
    const nlohmann::json input = {
        {"version", "1"},
        {"id", "controls"},
        {"metadata", {{"title", "Controls"}, {"tags", {"demo"}}}},
        {"canvas", {{"unit", "px"}}},
        {"sections", {{{"id", "form"}, {"nodes", {{{"id", "enabled"}, {"kind", "checkbox"},
            {"content", {{"text", "Enabled"}}}}}}}}},
        {"nodes", nlohmann::json::array()},
        {"relations", nlohmann::json::array()},
        {"data_sources", nlohmann::json::array()},
        {"initial_state", {{"enabled", true}}}
    };

    const auto document = input.get<exd::VisualDocument>();
    REQUIRE(document.sections.size() == 1);
    REQUIRE(document.sections[0].nodes.size() == 1);
    CHECK(document.sections[0].nodes[0].kind == exd::VisualNodeKind::Checkbox);
    CHECK(document.sections[0].nodes[0].content["text"] == "Enabled");
    CHECK(document.initial_state["enabled"] == true);
    CHECK_FALSE(document.sections[0].nodes[0].interaction.has_value());
    CHECK(document.density == "standard");
    CHECK_FALSE(document.composition.has_value());
}

TEST_CASE("VisualNode defaults to text when kind is omitted")
{
    const auto node = nlohmann::json{{"id", "body"}}.get<exd::VisualNode>();

    CHECK(node.kind == exd::VisualNodeKind::Text);
    CHECK(nlohmann::json(node)["kind"] == "text");
}

TEST_CASE("VisualNodeKind includes the orthographic 2D vocabulary")
{
    const std::vector<std::pair<exd::VisualNodeKind, const char*>> kinds = {
        {exd::VisualNodeKind::Shape, "shape"}, {exd::VisualNodeKind::Plot, "plot"},
        {exd::VisualNodeKind::Graph, "graph"}, {exd::VisualNodeKind::Legend, "legend"},
        {exd::VisualNodeKind::Form, "form"}, {exd::VisualNodeKind::Table, "table"},
        {exd::VisualNodeKind::Matrix, "matrix"}};
    for (const auto& [kind, name] : kinds)
        CHECK(nlohmann::json(kind) == name);
}

TEST_CASE("VisualDocumentPatch serializes structural document mutations")
{
    const exd::VisualDocumentPatch input{
        {
            {"add_node", "summary", {{"parent", "root"}},
                {{"id", "revenue"}, {"kind", "text"}, {"content", {{"text", "Q2"}}}}},
            {"remove_node", "old-chart", nullptr, nullptr},
            {"replace_node", "revenue", nullptr, {{"id", "revenue"}, {"kind", "chart"}}},
            {"update_content", "revenue", {{"merge", true}}, {{"text", "Revenue"}}},
            {"update_layout", "revenue", nullptr, {{"preset", "grid"}}},
            {"update_binding", "revenue", nullptr, {{"source", "sales"}}},
            {"update_interaction", "submit", nullptr, {{"clickable", true}}},
            {"add_relation", "relation-1", nullptr,
                {{"source", "revenue"}, {"target", "summary"}, {"kind", "controls"}}},
            {"remove_relation", "relation-0", nullptr, nullptr},
        }};

    const nlohmann::json json = input;
    CHECK(json["ops"].size() == 9);
    CHECK(json["ops"][0]["target"] == "summary");
    CHECK(json["ops"][0]["params"]["parent"] == "root");
    CHECK(json["ops"][3]["value"]["text"] == "Revenue");
    CHECK(json["ops"][7]["value"]["source"] == "revenue");
    CHECK_FALSE(json.dump().find("entity") != std::string::npos);
    CHECK_FALSE(json.dump().find("transform") != std::string::npos);

    const auto output = json.get<exd::VisualDocumentPatch>();
    REQUIRE(output.ops.size() == input.ops.size());
    CHECK(output.ops[0].op == "add_node");
    CHECK(output.ops[0].target == "summary");
    CHECK(output.ops[4].value["preset"] == "grid");
    CHECK(output.ops[8].op == "remove_relation");
}

TEST_CASE("VisualLayout/Placement/Node/Section carry the DOM-like vocabulary")
{
    SUBCASE("defaults")
    {
        const exd::VisualNode node;
        const exd::VisualSection section;
        const exd::VisualLayout layout;
        const exd::VisualPlacement placement;

        CHECK_FALSE(node.pass_through);
        CHECK_FALSE(section.pass_through);
        CHECK_FALSE(layout.weight.has_value());
        CHECK_FALSE(placement.absolute.has_value());
    }

    SUBCASE("full document round-trips the new vocabulary")
    {
        exd::VisualDocument doc;
        doc.id = "vocab-doc";

        exd::VisualLayout layout;
        layout.weight = 2.5f;
        layout.width = "fill";
        layout.min_width = "240";

        exd::VisualAbsolutePlacement abs;
        abs.x = "12%";
        abs.y = "0.5rem";

        exd::VisualPlacement placement;
        placement.absolute = abs;

        layout.placement = placement;

        exd::VisualNode node;
        node.id = "n1";
        node.layout = layout;
        node.pass_through = true;

        exd::VisualSection section;
        section.id = "s1";
        section.nodes.push_back(node);
        section.pass_through = true;

        doc.sections.push_back(section);

        const nlohmann::json json = doc;
        const auto round_trip = json.get<exd::VisualDocument>();

        REQUIRE(round_trip.sections.size() == 1);
        const auto& s = round_trip.sections[0];
        REQUIRE(s.nodes.size() == 1);
        const auto& n = s.nodes[0];

        CHECK(s.pass_through);
        CHECK(n.pass_through);
        REQUIRE(n.layout.has_value());
        REQUIRE(n.layout->weight.has_value());
        CHECK(n.layout->weight.value() == doctest::Approx(2.5f));
        CHECK(n.layout->width == "fill");
        CHECK(n.layout->min_width == "240");
        REQUIRE(n.layout->placement.has_value());
        REQUIRE(n.layout->placement->absolute.has_value());
        CHECK(n.layout->placement->absolute->x == "12%");
        CHECK(n.layout->placement->absolute->y == "0.5rem");
    }

    SUBCASE("json key presence")
    {
        exd::VisualLayout layout;
        layout.weight = 1.0f;
        exd::VisualNode node;
        node.id = "n";
        node.layout = layout;
        node.pass_through = true;
        exd::VisualSection section;
        section.id = "s";
        section.pass_through = true;
        exd::VisualDocument doc;
        doc.id = "d";
        doc.sections.push_back(section);
        doc.sections[0].nodes.push_back(node);

        const nlohmann::json json = doc;
        CHECK(json["sections"][0]["pass_through"] == true);
        CHECK(json["sections"][0]["nodes"][0]["pass_through"] == true);
        CHECK(json["sections"][0]["nodes"][0]["layout"]["weight"] == 1.0f);
    }
}
