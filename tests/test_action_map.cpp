#include "catch_amalgamated.hpp"
#include "core/input/action_map.hpp"

#include <GLFW/glfw3.h>

struct TestInput {
    std::unordered_set<int> heldKeys, pressedKeys, releasedKeys;
    std::unordered_set<int> heldButtons, pressedButtons, releasedButtons;
    glm::vec2 mouseDelta { 0.0f }, scrollDelta { 0.0f };
    RawInputState raw;

    TestInput()
        : raw { heldKeys, pressedKeys, releasedKeys,
                heldButtons, pressedButtons, releasedButtons,
                mouseDelta, scrollDelta } {}
};

TEST_CASE("Multiple keys bind to one action") {
    ActionMap map;
    map.addBinding("move_forward", { ActionType::Bool, InputSource::Key, GLFW_KEY_W });
    map.addBinding("move_forward", { ActionType::Bool, InputSource::Key, GLFW_KEY_UP });

    TestInput input;
    REQUIRE_FALSE(map.isActive("move_forward", input.raw));

    input.heldKeys.insert(GLFW_KEY_W);
    REQUIRE(map.isActive("move_forward", input.raw));

    input.heldKeys.clear();
    input.heldKeys.insert(GLFW_KEY_UP);
    REQUIRE(map.isActive("move_forward", input.raw));
}

TEST_CASE("Pressed and released edges fire per key") {
    ActionMap map;
    map.addBinding("jump", { ActionType::Bool, InputSource::Key, GLFW_KEY_SPACE });

    TestInput input;
    REQUIRE_FALSE(map.isPressed("jump", input.raw));
    REQUIRE_FALSE(map.isReleased("jump", input.raw));

    input.pressedKeys.insert(GLFW_KEY_SPACE);
    REQUIRE(map.isPressed("jump", input.raw));

    input.pressedKeys.clear();
    input.releasedKeys.insert(GLFW_KEY_SPACE);
    REQUIRE(map.isReleased("jump", input.raw));
}

TEST_CASE("Mouse button binding acts as a bool source") {
    ActionMap map;
    map.addBinding("select", { ActionType::Bool, InputSource::MouseButton, 0, GLFW_MOUSE_BUTTON_LEFT });

    TestInput input;
    input.heldButtons.insert(GLFW_MOUSE_BUTTON_LEFT);
    REQUIRE(map.isActive("select", input.raw));

    input.heldButtons.clear();
    input.pressedButtons.insert(GLFW_MOUSE_BUTTON_LEFT);
    REQUIRE(map.isPressed("select", input.raw));
}

TEST_CASE("Axis sums mouse deltas and applies scale") {
    ActionMap map;
    map.addBinding("look_x", { ActionType::Axis, InputSource::MouseAxis, 0, 0, 0 });
    map.addBinding("look_x_boosted", { ActionType::Axis, InputSource::MouseAxis, 0, 0, 0, 2.0f });

    TestInput input;
    input.mouseDelta = { 3.0f, -2.0f };

    REQUIRE(map.getAxis("look_x", input.raw) == Catch::Approx(3.0f));
    REQUIRE(map.getAxis("look_x_boosted", input.raw) == Catch::Approx(6.0f));
}

TEST_CASE("Scroll axis reads the Y delta") {
    ActionMap map;
    map.addBinding("zoom", { ActionType::Axis, InputSource::ScrollAxis, 0, 0, 1 });

    TestInput input;
    input.scrollDelta = { 0.0f, 5.0f };
    REQUIRE(map.getAxis("zoom", input.raw) == Catch::Approx(5.0f));
}

TEST_CASE("Unknown or cleared actions are inactive and zero") {
    ActionMap map;
    map.addBinding("temp", { ActionType::Bool, InputSource::Key, GLFW_KEY_A });

    TestInput input;
    input.heldKeys.insert(GLFW_KEY_A);

    REQUIRE_FALSE(map.isActive("does_not_exist", input.raw));
    REQUIRE_FALSE(map.isPressed("does_not_exist", input.raw));
    REQUIRE_FALSE(map.isReleased("does_not_exist", input.raw));
    REQUIRE(map.getAxis("does_not_exist", input.raw) == 0.0f);

    map.clearBindings("temp");
    REQUIRE_FALSE(map.isActive("temp", input.raw));
}
