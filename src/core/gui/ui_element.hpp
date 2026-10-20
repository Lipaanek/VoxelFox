#pragma once

#include <string>
#include <vector>
#include <functional>

enum class UIType {
    Text,
    Button,
    Checkbox,
    Window
};

struct UIElement {
    UIType type = UIType::Text;
    std::string text;
    std::string id;

    bool visible = true;
    bool enabled = true;
    bool checked = false;

    std::vector<UIElement> children;
    std::function<void()> onClick;
};