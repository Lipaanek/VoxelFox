#pragma once
#include "ui_element.hpp"

UIElement makeContainer(
    std::string id,
    UIVector2 position,
    UIVector2 size,
    std::vector<UIElement> children,
    UIAnchor anchor = UIAnchor::TopLeft
);
UIElement makeText(
    std::string id,
    std::string text,
    UIVector2 position = {},
    UIAnchor anchor = UIAnchor::TopLeft
);
UIElement makeButton(
    std::string id,
    std::string text,
    std::function<void()> callback = {},
    UIVector2 position = {},
    UIVector2 size = {},
    UIAnchor anchor = UIAnchor::TopLeft
);
UIElement makeCheckbox(
    std::string id,
    std::string text,
    std::function<void()> callback = {},
    bool checked = false,
    UIVector2 position = {},
    UIAnchor anchor = UIAnchor::TopLeft
);