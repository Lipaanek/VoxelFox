#pragma once

#include "ui_node.hpp"

class UIPanel : public UINode {
private:
    glm::vec3 color { 1.0f };
    float transparency = 1.0f;

public:
    UIPanel();
    explicit UIPanel(const std::string& name);

    [[nodiscard]] glm::vec3 getColor() const;
    void setColor(glm::vec3 newColor);

    [[nodiscard]] float getTransparency() const;
    void setTransparency(float newTransp);

    [[nodiscard]] UIType getUIType() const override;
};
