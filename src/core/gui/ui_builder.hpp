#pragma once
#include "ui_element.hpp"

UIElement makeWindow(std::string id, std::string title, std::vector<UIElement> children);
UIElement makeText(std::string id, std::string text);
UIElement makeButton(std::string id, std::string text, std::function<void()> callback = {});
UIElement makeCheckbox(std::string id, std::string text, std::function<void()> callback = {}, bool checked = false);