#include <memory>
#include <string_view>
#include <utility>

#include "core/camera/camera.hpp"
#include "core/input/input_system.hpp"
#include "core/renderer/renderer.hpp"
#include "core/scene/environment.hpp"
#include "core/window/window.hpp"
#include "core/scene/scene_manager.hpp"
#include "core/scene/editor.hpp"
#include "core/util/util.hpp"
#include "scene/demo_scene.hpp"

int main(const int argc, char* argv[]) {
    const std::string_view mode = argc > 1 ? argv[1] : "--editor";
    if (argc > 2 || (mode != "--editor" && mode != "--playback" && mode != "--export")) {
        Util::Log::error("Usage: voxelfox.exe [--editor|--playback|--export]");
        return 1;
    }

    constexpr WindowSettings settings {
        .width = 1920,
        .height = 1080,
        .title = "VoxelFox",
        .vsync = VSync::VSyncEnabled,
        .type = DisplayType::WindowMaximized,
    };

    Window window(settings);

    Camera camera;

    Renderer renderer;

    // Setup editor
    SceneManager sceneManager(window, renderer, camera);
    InputSystem input(window);
    std::unique_ptr<Environment> currentEnvironment;

    if (mode == "--editor") {
        currentEnvironment = std::make_unique<Editor>(
            sceneManager,
            window,
            input,
            camera
        );
    } else {
        currentEnvironment = std::make_unique<Environment>(EnvironmentObjects {
            .sceneManager = sceneManager,
            .window = window,
            .inputSystem = input,
            .camera = camera,
            .uiRenderMode = UIRenderMode::Runtime
        });
    }

    auto scene = createDemoScene();
    if (!scene)
        return 1;

    currentEnvironment->setScene(std::move(scene));

    if (auto* activeScene = sceneManager.getCurrentScene())
        attachDemoSceneScript(*activeScene);

    double lastTime = glfwGetTime();
    while (!window.shouldClose()) {
        const double currentTime = glfwGetTime();
        const auto dt = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        input.beginInput();
        window.update();

        currentEnvironment->beginUI();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render world
        currentEnvironment->update(dt);
        currentEnvironment->render();

        // Display frame
        window.present();
    }

    return 0;
}
