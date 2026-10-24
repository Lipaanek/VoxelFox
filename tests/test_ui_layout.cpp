#include "catch_amalgamated.hpp"

#include "core/gui/ui_builder.hpp"
#include "core/gui/ui_layout.hpp"
#include "core/scene/scene.hpp"

TEST_CASE("Runtime UI layout scales with viewport height") {
    UIElement element = makeContainer(
        "hud",
        { 12.0f, 24.0f },
        { 300.0f, 120.0f },
        {}
    );

    const UIRect layout = resolveUILayout(element, { 100.0f, 50.0f }, { 1920.0f, 1080.0f });

    REQUIRE(layout.position.x == Catch::Approx(112.0f));
    REQUIRE(layout.position.y == Catch::Approx(74.0f));
    REQUIRE(layout.size.x == Catch::Approx(300.0f));
    REQUIRE(layout.size.y == Catch::Approx(120.0f));
    REQUIRE(layout.scale == Catch::Approx(1.0f));
}

TEST_CASE("Runtime UI anchors align element edges to the viewport") {
    UIElement element = makeContainer(
        "hud",
        { -16.0f, -16.0f },
        { 200.0f, 100.0f },
        {},
        UIAnchor::BottomRight
    );

    const UIRect layout = resolveUILayout(element, { 0.0f, 0.0f }, { 1280.0f, 720.0f });

    REQUIRE(layout.scale == Catch::Approx(2.0f / 3.0f));
    REQUIRE(layout.position.x == Catch::Approx(1136.0f));
    REQUIRE(layout.position.y == Catch::Approx(642.6667f));
    REQUIRE(layout.size.x == Catch::Approx(400.0f / 3.0f));
    REQUIRE(layout.size.y == Catch::Approx(200.0f / 3.0f));
    REQUIRE(layout.pivot.x == Catch::Approx(1.0f));
    REQUIRE(layout.pivot.y == Catch::Approx(1.0f));
}

TEST_CASE("Child runtime UI anchors resolve within the parent container") {
    UIElement child = makeButton(
        "centered",
        "Click",
        {},
        { 0.0f, 0.0f },
        { 40.0f, 20.0f },
        UIAnchor::Center
    );

    const UIVector2 position = resolveUIElementPosition(child, { 200.0f, 100.0f });

    REQUIRE(position.x == Catch::Approx(80.0f));
    REQUIRE(position.y == Catch::Approx(40.0f));
}

TEST_CASE("Runtime UI elements are owned by their scene") {
    Scene scene;
    scene.addUIElement(makeText("title", "Scene UI"));

    REQUIRE(scene.getUIElements().size() == 1);
    REQUIRE(scene.getUIElements().front().text == "Scene UI");

    const Scene& constScene = scene;
    REQUIRE(constScene.getUIElements().front().id == "title");
}
