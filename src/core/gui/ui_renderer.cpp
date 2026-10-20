#include "ui_renderer.hpp"

#include "../window/window.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdexcept>
#include <utility>

#include "core/scripting/events/lua_event_bindings.hpp"
#include "core/util/util.hpp"

UIRenderer::UIRenderer(const Window& window) {
    GLFWwindow* const handle = window.getHandle();
    if (!handle || glfwGetCurrentContext() != handle) {
        Util::Log::error("UIRenderer requires the Window's OpenGL context to be current");
        return;
    }

    IMGUI_CHECKVERSION();
    context = ImGui::CreateContext();
    if (!context) {
        Util::Log::error("Failed to create ImGui context");
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = nullptr; // disable .ini file

    if (!ImGui_ImplGlfw_InitForOpenGL(handle, true)) {
        ImGui::DestroyContext(context);
        context = nullptr;

        Util::Log::error("Failed to initialize ImGui GLFW backend");
        return;
    }

    if (!ImGui_ImplOpenGL3_Init()) {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(context);
        context = nullptr;

        Util::Log::error("Failed to initialize ImGui OpenGL backend");
        return;
    }
}

UIRenderer::~UIRenderer() {
    if (!context)
        return;

    ImGui::SetCurrentContext(context);
    if (frameStarted)
        ImGui::EndFrame();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(context);
}

void UIRenderer::begin() {
    if (frameStarted) {
        Util::Log::warning("ImGui frame has already started");
        return;
    }

    ImGui::SetCurrentContext(context);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    frameStarted = true;
}

void UIRenderer::end() {
    if (!frameStarted) {
        Util::Log::warning("Cannot end an ImGui frame that has not started");
        return;
    }

    ImGui::SetCurrentContext(context);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    frameStarted = false;
}

void UIRenderer::renderElement(UIElement& element) {
    if (!frameStarted) {
        Util::Log::warning("Cannot render a UI element outside an ImGui frame");
        return;
    }

    ImGui::SetCurrentContext(context);
    std::vector<std::function<void()>> callbacks;
    renderElement(element, true, callbacks);

    for (const auto& callback : callbacks)
        callback();
}

void UIRenderer::renderElement(UIElement& element, const bool parentEnabled, std::vector<std::function<void()>>& callbacks) {
    if (!element.visible)
        return;

    const bool enabled = parentEnabled && element.enabled;

    if (!enabled)
        ImGui::BeginDisabled();

    if (!element.id.empty())
        ImGui::PushID(element.id.c_str());

    switch (element.type) {
        case UIType::Text:
            ImGui::TextUnformatted(element.text.data(), element.text.data() + element.text.size());
            break;

        case UIType::Button:
            if (ImGui::Button(element.text.c_str()) && element.onClick)
                callbacks.push_back(element.onClick);
            break;

        case UIType::Checkbox:
            if (ImGui::Checkbox(element.text.c_str(), &element.checked) && element.onClick)
                callbacks.push_back(element.onClick);
            break;

        case UIType::Window:
            if (ImGui::Begin(element.text.c_str(), &element.visible)) {
                for (UIElement& child : element.children)
                    renderElement(child, enabled, callbacks);
            }
            ImGui::End();
            break;
    }

    if (!element.id.empty())
        ImGui::PopID();

    if (!enabled)
        ImGui::EndDisabled();
}

void UIRenderer::render(std::vector<UIElement>& elements) {
    if (!frameStarted) {
        Util::Log::warning("Cannot render UI elements outside an ImGui frame");
        return;
    }

    ImGui::SetCurrentContext(context);
    std::vector<std::function<void()>> callbacks;

    for (UIElement& element : elements)
        renderElement(element, true, callbacks);

    for (const auto& callback : callbacks)
        callback();

    end();
}
