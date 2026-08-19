#pragma once

#include "../core/scene/scene.hpp"

class BasicScene : public Scene {

public:
    ~BasicScene() override;

protected:
    void onReady() override;
    void onUpdate(float dt) override;
};
