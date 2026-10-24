#include "ui_layout.hpp"

namespace {

UIVector2 anchorFactor(const UIAnchor anchor) {
    switch (anchor) {
        case UIAnchor::TopLeft:
            return { 0.0f, 0.0f };
        case UIAnchor::Top:
            return { 0.5f, 0.0f };
        case UIAnchor::TopRight:
            return { 1.0f, 0.0f };
        case UIAnchor::Left:
            return { 0.0f, 0.5f };
        case UIAnchor::Center:
            return { 0.5f, 0.5f };
        case UIAnchor::Right:
            return { 1.0f, 0.5f };
        case UIAnchor::BottomLeft:
            return { 0.0f, 1.0f };
        case UIAnchor::Bottom:
            return { 0.5f, 1.0f };
        case UIAnchor::BottomRight:
            return { 1.0f, 1.0f };
    }

    return {};
}

}

UIVector2 resolveUIElementPosition(const UIElement& element, const UIVector2 parentSize) {
    const UIVector2 pivot = anchorFactor(element.anchor);
    return {
        parentSize.x * pivot.x + element.position.x - element.size.x * pivot.x,
        parentSize.y * pivot.y + element.position.y - element.size.y * pivot.y
    };
}

UIRect resolveUILayout(
    const UIElement& element,
    const UIVector2 viewportPosition,
    const UIVector2 viewportSize
) {
    const float scale = viewportSize.y > 0.0f
        ? viewportSize.y / UI_LOGICAL_CANVAS_HEIGHT
        : 0.0f;
    const UIVector2 pivot = anchorFactor(element.anchor);
    const UIVector2 logicalViewportSize {
        scale > 0.0f ? viewportSize.x / scale : 0.0f,
        UI_LOGICAL_CANVAS_HEIGHT
    };
    const UIVector2 logicalTopLeft = resolveUIElementPosition(element, logicalViewportSize);

    return {
        .position = {
            viewportPosition.x + logicalTopLeft.x * scale,
            viewportPosition.y + logicalTopLeft.y * scale
        },
        .size = {
            element.size.x * scale,
            element.size.y * scale
        },
        .pivot = pivot,
        .scale = scale
    };
}
