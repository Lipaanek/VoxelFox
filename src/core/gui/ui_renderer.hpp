#pragma once

#include <vector>

#include "ui_element.hpp"

class Window;
struct ImGuiContext;

class UIRenderer {
private:
    ImGuiContext* context = nullptr;
    bool frameStarted = false;

    void renderElement(UIElement& element, bool parentEnabled, std::vector<std::function<void()>>& callbacks);

public:
    explicit UIRenderer(const Window& window);
    ~UIRenderer();

    UIRenderer(const UIRenderer&) = delete;
    UIRenderer& operator=(const UIRenderer&) = delete;
    UIRenderer(UIRenderer&&) = delete;
    UIRenderer& operator=(UIRenderer&&) = delete;

    void begin();
    void end();
    void renderElement(UIElement& element);
    void render(std::vector<UIElement>& elements);
};
