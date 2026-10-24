#include "ui_builder.hpp"

#include <utility>

UIElement makeContainer(
    std::string id,
    const UIVector2 position,
    const UIVector2 size,
    std::vector<UIElement> children,
    const UIAnchor anchor
) {
    UIElement element {
        .type = UIType::Container,
        .id = std::move(id),
        .position = position,
        .size = size,
        .anchor = anchor,
        .children = std::move(children),
    };

    return element;
}

UIElement makeText(std::string id, std::string text, const UIVector2 position, const UIAnchor anchor) {
    UIElement element {
        .type = UIType::Text,
        .text = std::move(text),
        .id = std::move(id),
        .position = position,
        .anchor = anchor,
    };

    return element;
}

UIElement makeButton(
    std::string id,
    std::string text,
    std::function<void()> callback,
    const UIVector2 position,
    const UIVector2 size,
    const UIAnchor anchor
) {
    UIElement element {
        .type = UIType::Button,
        .text = std::move(text),
        .id = std::move(id),
        .position = position,
        .size = size,
        .anchor = anchor,
        .onClick = std::move(callback),
    };

    return element;
}

UIElement makeCheckbox(
    std::string id,
    std::string text,
    std::function<void()> callback,
    const bool checked,
    const UIVector2 position,
    const UIAnchor anchor
) {
    UIElement element {
        .type = UIType::Checkbox,
        .text = std::move(text),
        .id = std::move(id),
        .position = position,
        .anchor = anchor,
        .checked = checked,
        .onClick = std::move(callback),
    };

    return element;
}
