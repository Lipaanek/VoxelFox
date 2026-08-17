#pragma once

#include "../core/scene/scene.hpp"

class BasicScene : public Scene {

public:
    ~BasicScene() override;

    void ready() override;
    void update(float dt) override;
};
