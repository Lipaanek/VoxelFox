#pragma once

#include "ui_element.hpp"

struct UIRect {
    UIVector2 position;
    UIVector2 size;
    UIVector2 pivot;
    float scale = 1.0f;
};

inline constexpr float UI_LOGICAL_CANVAS_HEIGHT = 1080.0f;

UIVector2 resolveUIElementPosition(const UIElement& element, UIVector2 parentSize);

UIRect resolveUILayout(
    const UIElement& element,
    UIVector2 viewportPosition,
    UIVector2 viewportSize
);
