#pragma once

#include "transform2d.hpp"
#include "../node.hpp"

enum class UIType {
    None,
    Panel,
    Image,
    Text,
    Button
};

class UINode : public Node {
public:
    UINode();
    ~UINode() override = default;
    explicit UINode(const std::string& name);

    void setPosition(glm::vec2 newPosition);
    void setScale(glm::vec2 newScale);
    void setDimensions(glm::vec2 newDims);
    void setAnchor(glm::vec2 newAnchor);

    [[nodiscard]] glm::vec2 getPosition() const;
    [[nodiscard]] glm::vec2 getScale() const;
    [[nodiscard]] UINode* getParent() const override;
    [[nodiscard]] Transform2D getTransform() const;
    [[nodiscard]] glm::vec2 getDimensions() const;
    [[nodiscard]] glm::vec2 getAnchor() const;

    [[nodiscard]] virtual UIType getUIType() const = 0;

protected:
    Transform2D transform;
};
