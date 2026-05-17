#include "project_config.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <filesystem>

ProjectConfig ProjectConfigLoader::Load(const std::string& projectPath) {
    ProjectConfig config;
    
    // Construct path to project.vfp file
    std::filesystem::path vfpPath = std::filesystem::path(projectPath) / "project.vfp";
    
    std::ifstream file(vfpPath);
    if (!file.is_open()) {
        printf("Warning: Could not open project.vfp at %s, using defaults\n", vfpPath.string().c_str());
        return config;  // Return defaults
    }

    std::string line;
    while (std::getline(file, line)) {
        ParseLine(line, config);
    }

    file.close();
    return config;
}

void ProjectConfigLoader::ParseLine(const std::string& line, ProjectConfig& config) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
        return;
    }

    // Find the '=' character
    size_t eqPos = line.find('=');
    if (eqPos == std::string::npos) {
        return;  // No '=' found, skip
    }

    // Extract key and value, trim whitespace
    std::string key = line.substr(0, eqPos);
    std::string value = line.substr(eqPos + 1);

    // Trim leading/trailing whitespace from key
    key.erase(0, key.find_first_not_of(" \t\r\n"));
    key.erase(key.find_last_not_of(" \t\r\n") + 1);

    // Trim leading/trailing whitespace and quotes from value
    value.erase(0, value.find_first_not_of(" \t\r\n\""));
    value.erase(value.find_last_not_of(" \t\r\n\"") + 1);

    // Parse known keys
    if (key == "project_name") {
        config.projectName = value;
    } else if (key == "voxel_size") {
        try {
            config.voxelSize = std::stof(value);
        } catch (const std::exception& e) {
            printf("Warning: Could not parse voxel_size value '%s': %s\n", value.c_str(), e.what());
        }
    }
}
