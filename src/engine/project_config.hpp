#pragma once

#include <string>

struct ProjectConfig {
    std::string projectName;
    float voxelSize = 0.1f;
};

class ProjectConfigLoader {
public:
    static ProjectConfig Load(const std::string& projectPath);

private:
    static void ParseLine(const std::string& line, ProjectConfig& config);
};
