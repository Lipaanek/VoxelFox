#pragma once

#include <string>

struct ProjectConfig {
    std::string projectName;
    float voxelSize = 0.1f;
};

class ProjectConfigLoader {
public:
    /**
     * Load project configuration from project.vfp file
     * @param projectPath Full path to the project directory
     * @return ProjectConfig struct with loaded values
     */
    static ProjectConfig Load(const std::string& projectPath);

private:
    /**
     * Parse a single line from project.vfp file
     * Updates config based on key=value pairs
     * @param line The line to parse
     * @param config The config to update
     */
    static void ParseLine(const std::string& line, ProjectConfig& config);
};
