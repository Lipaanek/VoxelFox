#include "ui_element.hpp"

class Panel : public UIElement {
public:
    Panel(glm::vec2 position,
          glm::vec2 size,
          glm::vec4 color,
          float radius)
        : UIElement(position, size),
          m_color(color),
          m_radius(radius)
    {
    }

    void draw(UIRenderer& renderer) override;

    void setColor(glm::vec4 color) { m_color = color; }
    void setRadius(float radius) { m_radius = radius; }

private:
    glm::vec4 m_color;
    float m_radius;
};