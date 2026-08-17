#pragma once

#include "scene_manager.hpp"
#include "../camera/camera.hpp"
#include "../window/window.hpp"

class Playtest {
private:
    SceneManager *sceneManager;
    Window &window;
    Camera camera;

public:
    Playtest(SceneManager *sceneManager, Window &window);

    Camera &getCamera();

    void update(float dt) const;
    void render(const RenderContext &ctx) const;
};
