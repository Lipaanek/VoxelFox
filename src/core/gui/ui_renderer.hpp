#pragma once

#include <functional>
#include <vector>

#include "ui_element.hpp"

class Window;
struct ImGuiContext;

struct UICaptureState {
    bool mouse = false;
    bool keyboard = false;
};

enum class UIRenderMode {
    Editor,
    Runtime
};

class UIRenderer {
private:
    ImGuiContext* context = nullptr;
    bool frameStarted = false;

    void renderElement(
        UIElement& element,
        bool parentEnabled,
        std::vector<std::function<void()>>& callbacks,
        UIVector2 parentSize,
        float scale,
        UIRenderMode mode,
        bool root
    );

public:
    explicit UIRenderer(const Window& window);
    ~UIRenderer();

    UIRenderer(const UIRenderer&) = delete;
    UIRenderer& operator=(const UIRenderer&) = delete;
    UIRenderer(UIRenderer&&) = delete;
    UIRenderer& operator=(UIRenderer&&) = delete;

    UICaptureState begin();
    void renderRuntimeUI(std::vector<UIElement>& elements, UIRenderMode mode);
    void renderEditorDockspace();
    UICaptureState end();
};
