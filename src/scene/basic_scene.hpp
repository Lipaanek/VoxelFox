#pragma once

#include "../core/scene/scene.hpp"

class BasicScene : public Scene {

public:
    ~BasicScene() override;

    void update(float dt) override;

protected:
    void onReady() override;
};
