#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "engine/gui/screen_manager.hpp"
#include "engine/gui/screens/create_project_screen.hpp"
#include "engine/gui/screens/main_editor_screen.hpp"
#include "engine/gui/screens/playtest_screen.hpp"
#include "engine/scene_hierarchy/scene.hpp"

// Camera globals (set by GLFW callbacks, consumed by MainEditorScreen)
float velocityX = 0.0f;
float velocityY = 0.0f;
float velocityZ = 0.0f;
const float speed = 10.0f;
const float sens = 0.2f;
double prevMouseX = 0.0;
double prevMouseY = 0.0;
float yaw = 0.0f;
float pitch = 0.0f;
bool editorRmbHeld = false;
bool editorFirstMouse = true;
double cursorLockX = 0.0;
double cursorLockY = 0.0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ScreenManager* screenMgr = static_cast<ScreenManager*>(glfwGetWindowUserPointer(window));
    if (screenMgr) {
        Screen* current = screenMgr->GetCurrent();
        if (current && button == GLFW_MOUSE_BUTTON_RIGHT) {
            PlaytestScreen* playtest = dynamic_cast<PlaytestScreen*>(current);
            if (playtest) {
                if (action == GLFW_PRESS) {
                    playtest->HandleRMB(true);
                }
                return;
            }

            MainEditorScreen* editor = dynamic_cast<MainEditorScreen*>(current);
            if (editor) {
                if (action == GLFW_PRESS) {
                    editorRmbHeld = true;
                    editorFirstMouse = true;
                    glfwGetCursorPos(window, &cursorLockX, &cursorLockY);
                    prevMouseX = cursorLockX;
                    prevMouseY = cursorLockY;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                } else if (action == GLFW_RELEASE) {
                    editorRmbHeld = false;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    glfwSetCursorPos(window, cursorLockX, cursorLockY);
                }
            }
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    ScreenManager* screenMgr = static_cast<ScreenManager*>(glfwGetWindowUserPointer(window));
    if (screenMgr) {
        Screen* current = screenMgr->GetCurrent();
        if (current) {
            PlaytestScreen* playtest = dynamic_cast<PlaytestScreen*>(current);
            if (playtest && playtest->IsMouseCaptured()) {
                return;
            }
        }
    }

    if (ImGui::GetIO().WantCaptureMouse) {
        prevMouseX = xpos;
        prevMouseY = ypos;
        return;
    }

    if (!editorRmbHeld) {
        prevMouseX = xpos;
        prevMouseY = ypos;
        return;
    }

    if (editorFirstMouse) {
        prevMouseX = xpos;
        prevMouseY = ypos;
        editorFirstMouse = false;
        return;
    }

    double dx = prevMouseX - xpos;
    double dy = prevMouseY - ypos;

    yaw += (float) dx * sens * -1;
    pitch += (float) dy * sens;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    prevMouseX = xpos;
    prevMouseY = ypos;
}

void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard)                   return;
    if (key == GLFW_KEY_A && action == GLFW_PRESS)            velocityX = -speed;
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)          velocityX = 0.0f;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)            velocityX = speed;
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)          velocityX = 0.0f;
    if (key == GLFW_KEY_W && action == GLFW_PRESS)            velocityY = speed;
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)          velocityY = 0.0f;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)            velocityY = -speed;
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)          velocityY = 0.0f;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)        velocityZ = speed;
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)      velocityZ = 0.0f;
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)   velocityZ = -speed;
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) velocityZ = 0.0f;
}

void initImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

int main() {
    if (!glfwInit()) {
        printf("Failed to init GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "VoxelFox", NULL, NULL);
    if (!window) {
        printf("Failed to create window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glEnable(GL_DEPTH_TEST);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, input_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    initImGui(window);

    ScreenManager screenManager;
    CreateProjectScreen createScreen;
    glfwSetWindowUserPointer(window, &screenManager);

    MeshManager meshMgr;
    Scene scene;
    PlaytestScreen playtestScreen(window, "", scene, meshMgr);
    MainEditorScreen editorScreen(window, scene, meshMgr);

    playtestScreen.SetReturnCallback([&]() {
        editorRmbHeld = false;
        screenManager.SwitchTo(&editorScreen);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    });

    editorScreen.onEnterPlaytest_f = [&]() {
        editorRmbHeld = false;
        playtestScreen.SetProjectPath(editorScreen.GetProjectPath());
        screenManager.SwitchTo(&playtestScreen);
    };

    createScreen.OnProjectCreated = [&]() {
        editorScreen.SetProjectPath(createScreen.GetProjectPath());
        
        glfwMaximizeWindow(window);
        screenManager.SwitchTo(&editorScreen);
    };
    createScreen.OnProjectOpened = [&]() {
        editorScreen.SetProjectPath(createScreen.GetProjectPath());

        glfwMaximizeWindow(window);
        screenManager.SwitchTo(&editorScreen);
    };

    screenManager.SwitchTo(&createScreen);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        screenManager.UpdateCurrent();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        screenManager.RenderCurrent();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        glfwSwapBuffers(window);
    }

    if (screenManager.GetCurrent()) {
        screenManager.GetCurrent()->OnExit();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
