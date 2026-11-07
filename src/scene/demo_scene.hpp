#pragma once

#include <memory>

class Scene;

std::unique_ptr<Scene> createDemoScene();
void attachDemoSceneScript(Scene& scene);
