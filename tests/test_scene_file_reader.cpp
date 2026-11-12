#include "catch_amalgamated.hpp"

#include "core/readable_files/scene_file_reader.hpp"
#include <iostream>

TEST_CASE("Loading basic test scene") {
    const std::string source = R"(
        [node]
        name = "Root"
        type = "Node3D"
        position = [0, 0, 0]
        visible = true

        [node]
        name = "Camera"
        type = "Camera3D"
        position = [0, 2, -5]
    )";

    SceneFileReader sceneReader(source);

    auto scene = sceneReader.parse();
    CHECK(scene.size() == 2);
    // std::cout << "Parsed sections: " << scene.size() << '\n';
    //
    // for (const auto& section : scene) {
    //     CHECK(section.name == "node");
    //
    //     std::cout << "Section: " << section.name << '\n';
    //
    //     for (const auto& [key, value] : section.properties) {
    //         std::cout << "  Property: " << key << '\n';
    //     }
    // }
}

TEST_CASE("Loading nested arrays") {
    const std::string source = R"(
        [node]
        matrix = [[1, 0], [0, 1]]
    )";

    SceneFileReader reader(source);
    auto scene = reader.parse();

    REQUIRE(scene.size() == 1);

    const auto& matrix = std::get<SceneFileValue::Array>(
        scene[0].properties.at("matrix").value
    );

    REQUIRE(matrix.size() == 2);

    const auto& firstRow = std::get<SceneFileValue::Array>(
        matrix[0].value
    );

    REQUIRE(firstRow.size() == 2);
    CHECK(std::get<double>(firstRow[0].value) == 1.0);
    CHECK(std::get<double>(firstRow[1].value) == 0.0);
}