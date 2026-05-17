#pragma once

#include "../screen.hpp"
#include <functional>

class CreateProjectScreen : public Screen {
    public:
        void Update() override;
        void Render() override;

        std::function<void()> OnProjectCreated;

    private:
        char projectName[128] = "";
        char projectPath[128] = "";
        bool projectCreated = false;
};
