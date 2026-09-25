#include "ui_node.hpp"

UINode::UINode() {
    this->setName("UINode");
}

UINode::UINode(const std::string &name) {
    this->name = name;
}

UINode* UINode::getParent() const {
    return dynamic_cast<UINode*>(parent);
}

glm::vec2 UINode::getPosition() const {
    return this->transform.position;
}

glm::vec2 UINode::getScale() const {
    return this->transform.scale;
}

void UINode::setPosition(const glm::vec2 newPosition) {
    this->transform.position = newPosition;
}

void UINode::setScale(const glm::vec2 newScale) {
    this->transform.scale = newScale;
}

void UINode::setAnchor(const glm::vec2 newAnchor) {
    this->transform.anchor = newAnchor;
}

void UINode::setDimensions(const glm::vec2 newDims) {
    this->transform.dimensions = newDims;
}

Transform2D UINode::getTransform() const {
    return this->transform;
}

UIType UINode::getUIType() const {
    return UIType::None;
}

glm::vec2 UINode::getAnchor() const {
    return this->transform.anchor;
}

glm::vec2 UINode::getDimensions() const {
    return this->transform.dimensions;
}