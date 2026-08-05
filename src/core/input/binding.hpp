#pragma once

enum class ActionType {
    Bool,
    Axis
};

enum class InputSource {
    Key,
    MouseButton,
    MouseAxis,
    ScrollAxis
};

struct Binding {
    ActionType type;
    InputSource src;
    int key = 0;    // GLFW_KEY_*
    int button = 0; // GLFW_MOUSE_BUTTON_*
    int axis = 0;   // 0 = X; 1 = Y
    float scale = 1.0f;
};