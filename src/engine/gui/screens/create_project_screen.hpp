#pragma once

#include "../screen.hpp"

class CreateProjectScreen : public Screen {
    public:
        void Update() override;
        void Render() override;
    private:
        char projectName[128];
        char projectPath[128];
};