#pragma once

#include <string>
#include <vector>
#include <functional>

struct UIVector2 {
    float x = 0.0f;
    float y = 0.0f;
};

enum class UIAnchor {
    TopLeft,
    Top,
    TopRight,
    Left,
    Center,
    Right,
    BottomLeft,
    Bottom,
    BottomRight
};

enum class UIType {
    Container,
    Text,
    Button,
    Checkbox
};

struct UIElement {
    UIType type = UIType::Text;
    std::string text;
    std::string id;
    UIVector2 position;
    UIVector2 size;
    UIAnchor anchor = UIAnchor::TopLeft;

    bool visible = true;
    bool enabled = true;
    bool checked = false;

    std::vector<UIElement> children;
    std::function<void()> onClick;
};