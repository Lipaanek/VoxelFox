#pragma once

#include "../screen.hpp"
#include <functional>
#include <string>

class CreateProjectScreen : public Screen {
    public:
        void Update() override;
        void Render() override;

        std::function<void()> OnProjectCreated;

        // Getters for project information
        std::string GetProjectPath() const { return std::string(projectPath); }
        std::string GetProjectName() const { return std::string(projectName); }

    private:
        char projectName[128] = "";
        char projectPath[128] = "";
        bool projectCreated = false;
};
