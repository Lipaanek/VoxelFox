#pragma once

#include "scene_manager.hpp"
#include "../camera/camera.hpp"
#include "../window/window.hpp"

class Editor {
private:
    SceneManager *sceneManager;
    Window &window;
    Camera camera;

public:
    Editor(SceneManager *sceneManager, Window &window);

    Camera &getCamera();

    void update(float dt) const;
    void render(const RenderContext& ctx) const;
};
