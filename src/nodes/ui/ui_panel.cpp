#include "ui_panel.hpp"

UIPanel::UIPanel() {
    this->setName("UIPanel");
}

UIType UIPanel::getUIType() const {
    return UIType::Panel;
}

UIPanel::UIPanel(const std::string &name) {
    this->setName(name);
}

glm::vec3 UIPanel::getColor() const {
    return this->color;
}

void UIPanel::setColor(const glm::vec3 newColor) {
    this->color = newColor;
}

float UIPanel::getTransparency() const {
    return this->transparency;
}

void UIPanel::setTransparency(const float newTransp) {
    this->transparency = newTransp;
}
