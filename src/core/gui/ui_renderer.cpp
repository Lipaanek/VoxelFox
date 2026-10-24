#include "ui_renderer.hpp"

#include "../window/window.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <utility>

#include "core/util/util.hpp"
#include "ui_layout.hpp"

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
    io.Fonts->AddFontFromFileTTF("assets/fonts/Inter-Regular.ttf", 18.0f);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = nullptr;

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

UICaptureState UIRenderer::begin() {
    if (!context)
        return {};

    if (frameStarted) {
        Util::Log::warning("ImGui frame has already started");
        return {};
    }

    ImGui::SetCurrentContext(context);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    frameStarted = true;
    const ImGuiIO& io = ImGui::GetIO();
    return {
        .mouse = io.WantCaptureMouse,
        .keyboard = io.WantCaptureKeyboard
    };
}

void UIRenderer::renderElement(
    UIElement& element,
    const bool parentEnabled,
    std::vector<std::function<void()>>& callbacks,
    const UIVector2 parentSize,
    const float scale,
    const UIRenderMode mode,
    const bool root
) {
    if (!element.visible)
        return;

    const bool enabled = parentEnabled && element.enabled;
    if (!enabled)
        ImGui::BeginDisabled();

    if (!element.id.empty())
        ImGui::PushID(element.id.c_str());

    if (root) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const UIRect layout = resolveUILayout(
            element,
            { viewport->Pos.x, viewport->Pos.y },
            { viewport->Size.x, viewport->Size.y }
        );
        const ImVec2 anchorPosition {
            layout.position.x + layout.size.x * layout.pivot.x,
            layout.position.y + layout.size.y * layout.pivot.y
        };

        ImGui::SetNextWindowPos(
            anchorPosition,
            mode == UIRenderMode::Editor ? ImGuiCond_FirstUseEver : ImGuiCond_Always,
            { layout.pivot.x, layout.pivot.y }
        );
        if (element.size.x > 0.0f || element.size.y > 0.0f)
            ImGui::SetNextWindowSize(
                { layout.size.x, layout.size.y },
                mode == UIRenderMode::Editor ? ImGuiCond_FirstUseEver : ImGuiCond_Always
            );

        ImGui::PushStyleVar(
            ImGuiStyleVar_WindowPadding,
            element.type == UIType::Container
                ? ImVec2(8.0f * scale, 8.0f * scale)
                : ImVec2(0.0f, 0.0f)
        );
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f * scale, 3.0f * scale });
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 8.0f * scale, 4.0f * scale });

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoSavedSettings;
        if (mode == UIRenderMode::Runtime) {
            flags |= ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoFocusOnAppearing;
        }
        if (element.type != UIType::Container)
            flags |= ImGuiWindowFlags_NoBackground;
        if (element.type == UIType::Text)
            flags |= ImGuiWindowFlags_NoInputs;
        if (element.size.x <= 0.0f && element.size.y <= 0.0f)
            flags |= ImGuiWindowFlags_AlwaysAutoResize;

        std::string title = element.id.empty() ? "Runtime UI" : element.id;
        title += "##runtime_ui";
        const bool visible = ImGui::Begin(title.c_str(), nullptr, flags);
        if (visible) {
            ImGui::SetWindowFontScale(scale);

            switch (element.type) {
                case UIType::Container:
                {
                    const UIVector2 childArea {
                        element.size.x > 16.0f ? element.size.x - 16.0f : 0.0f,
                        element.size.y > 16.0f ? element.size.y - 16.0f : 0.0f
                    };
                    for (std::size_t i = 0; i < element.children.size(); ++i) {
                        ImGui::PushID(static_cast<int>(i));
                        renderElement(element.children[i], enabled, callbacks, childArea, scale, mode, false);
                        ImGui::PopID();
                    }
                    break;
                }
                case UIType::Text:
                    ImGui::TextUnformatted(
                        element.text.data(),
                        element.text.data() + element.text.size()
                    );
                    break;
                case UIType::Button:
                    if (ImGui::Button(
                            element.text.c_str(),
                            { element.size.x * scale, element.size.y * scale }
                        ) && element.onClick) {
                        callbacks.push_back(element.onClick);
                    }
                    break;
                case UIType::Checkbox:
                    if (ImGui::Checkbox(element.text.c_str(), &element.checked) && element.onClick)
                        callbacks.push_back(element.onClick);
                    break;
            }
        }

        ImGui::End();
        ImGui::PopStyleVar(3);
    } else if (element.type == UIType::Container) {
        const ImVec2 cursorStart = ImGui::GetCursorStartPos();
        const UIVector2 position = resolveUIElementPosition(element, parentSize);
        ImGui::SetCursorPos({
            cursorStart.x + position.x * scale,
            cursorStart.y + position.y * scale
        });

        const bool visible = ImGui::BeginChild(
            "##runtime_ui_container",
            { element.size.x * scale, element.size.y * scale },
            ImGuiChildFlags_None,
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking
        );
        if (visible) {
            ImGui::SetWindowFontScale(scale);
            const UIVector2 childArea {
                element.size.x > 16.0f ? element.size.x - 16.0f : 0.0f,
                element.size.y > 16.0f ? element.size.y - 16.0f : 0.0f
            };
            for (std::size_t i = 0; i < element.children.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));
                renderElement(element.children[i], enabled, callbacks, childArea, scale, mode, false);
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    } else {
        const ImVec2 cursorStart = ImGui::GetCursorStartPos();
        const UIVector2 position = resolveUIElementPosition(element, parentSize);
        ImGui::SetCursorPos({
            cursorStart.x + position.x * scale,
            cursorStart.y + position.y * scale
        });

        switch (element.type) {
            case UIType::Text:
                ImGui::TextUnformatted(
                    element.text.data(),
                    element.text.data() + element.text.size()
                );
                break;
            case UIType::Button:
                if (ImGui::Button(
                        element.text.c_str(),
                        { element.size.x * scale, element.size.y * scale }
                    ) && element.onClick) {
                    callbacks.push_back(element.onClick);
                }
                break;
            case UIType::Checkbox:
                if (ImGui::Checkbox(element.text.c_str(), &element.checked) && element.onClick)
                    callbacks.push_back(element.onClick);
                break;
            case UIType::Container:
                break;
        }
    }

    if (!element.id.empty())
        ImGui::PopID();

    if (!enabled)
        ImGui::EndDisabled();
}

void UIRenderer::renderRuntimeUI(std::vector<UIElement>& elements, const UIRenderMode mode) {
    if (!frameStarted) {
        Util::Log::warning("Cannot render runtime UI outside an ImGui frame");
        return;
    }

    ImGui::SetCurrentContext(context);
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (!viewport || viewport->Size.y <= 0.0f)
        return;

    const float scale = viewport->Size.y / UI_LOGICAL_CANVAS_HEIGHT;
    std::vector<std::function<void()>> callbacks;
    const UIVector2 logicalViewportSize {
        viewport->Size.x / scale,
        UI_LOGICAL_CANVAS_HEIGHT
    };

    for (std::size_t i = 0; i < elements.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));
        renderElement(elements[i], true, callbacks, logicalViewportSize, scale, mode, true);
        ImGui::PopID();
    }

    for (const auto& callback : callbacks)
        callback();
}

void UIRenderer::renderEditorDockspace() {
    if (!frameStarted)
        return;

    ImGui::SetCurrentContext(context);
    ImGui::DockSpaceOverViewport(
        0,
        ImGui::GetMainViewport(),
        ImGuiDockNodeFlags_PassthruCentralNode
    );
}

UICaptureState UIRenderer::end() {
    if (!context || !frameStarted) {
        Util::Log::warning("Cannot end an ImGui frame that has not started");
        return {};
    }

    ImGui::SetCurrentContext(context);
    const ImGuiIO& io = ImGui::GetIO();
    const UICaptureState capture {
        .mouse = io.WantCaptureMouse,
        .keyboard = io.WantCaptureKeyboard
    };

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    frameStarted = false;
    return capture;
}
