#pragma once

#include <string>
#include "project_file.hpp"

// Project file example
//
// ; VoxelFox engine configuration file.
// ; It's best edited using the editor UI and not directly
// [application]
// version = "1.9.2"
// project_name = "My Awesome Game"
//
// [input]
// left = {
//      "A"
//      "left" ; left arrow
// }

class ProjectFileHandler {
private:
    // In-memory project data; call save() to write changes to the project file.
    // Retrieving value from env is easy:
    //
    // auto& value = environment["application"]["project_name"];
    ProjectFile& m_projectFile;

    void buildProjectFile() const;

public:
    [[nodiscard]] std::string valueToString(const VariableValue &value) const;
    [[nodiscard]] ProjectFile& getEnv() const;

    explicit ProjectFileHandler(ProjectFile& projectFile);

    void load() const;
    void load(const std::filesystem::path& filePath) const;
    void save() const;

    void addVariable(const std::string& sectionName, const std::string& variableName, const VariableValue &value) const;
    void removeVariable(const std::string& sectionName, const std::string& variableName) const;

    void addSection(const std::string& sectionName) const;
    void removeSection(const std::string& sectionName) const;
};
