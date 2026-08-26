#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class UIRenderer;

class UIElement {
public:
    UIElement(glm::vec2 position, glm::vec2 size)
        : m_position(position),
          m_size(size)
    {}

    virtual ~UIElement() = default;

    virtual void draw(UIRenderer& renderer) = 0;

    // Position
    void setPosition(glm::vec2 position) { m_position = position; }
    glm::vec2 getPosition() const { return m_position; }

    // Size
    void setSize(glm::vec2 size) { m_size = size; }
    glm::vec2 getSize() const { return m_size; }

    // Visibility
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

protected:
    glm::vec2 m_position{};
    glm::vec2 m_size{};

    bool m_visible = true;

};

