#include "ui_builder.hpp"

#include <utility>

UIElement makeButton(std::string id, std::string text, std::function<void()> callback) {
    UIElement element {
        .type = UIType::Button,
        .text = std::move(text),
        .id = std::move(id),
        .onClick = std::move(callback),
    };

    return element;
}

UIElement makeCheckbox(std::string id, std::string text, std::function<void()> callback, const bool checked) {
    UIElement element {
        .type = UIType::Checkbox,
        .text = std::move(text),
        .id = std::move(id),
        .checked = checked,
        .onClick = std::move(callback),
    };

    return element;
}

UIElement makeText(std::string id, std::string text) {
    UIElement element {
        .type = UIType::Text,
        .text = std::move(text),
        .id = std::move(id),
    };

    return element;
}

UIElement makeWindow(std::string id, std::string title, std::vector<UIElement> children) {
    UIElement element {
        .type = UIType::Window,
        .text = std::move(title),
        .id = std::move(id),
        .children = std::move(children),
    };

    return element;
}
