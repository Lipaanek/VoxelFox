#include "project_file_handler.hpp"

#include <format>
#include <type_traits>

#include "core/util/util.hpp"

std::string ProjectFileHandler::valueToString(const VariableValue& value) const {
    return std::visit([]<typename T0>(const T0& v) -> std::string {
        using T = std::decay_t<T0>;

        if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            std::string result = "\"";
            for (const char character : v) {
                switch (character) {
                case '\\': result += "\\\\"; break;
                case '"': result += "\\\""; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += character; break;
                }
            }
            result += '"';
            return result;
        }
        else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>) {
            return std::format("{}", v);
        } else {
            Util::Log::error("Unsupported VariableValue type");
            return "ERR";
        }
    }, value);
}

ProjectFileHandler::ProjectFileHandler(ProjectFile& projectFile) : projectFile(projectFile) {}

void ProjectFileHandler::addSection(const std::string& sectionName) const {
    projectFile.environment.try_emplace(sectionName);
}

void ProjectFileHandler::removeSection(const std::string& sectionName) const {
    if (this->projectFile.environment.find(sectionName) == this->projectFile.environment.end()) return;

    this->projectFile.environment.erase(sectionName);
}

void ProjectFileHandler::addVariable(const std::string& sectionName, const std::string& variableName, const VariableValue& value) const {
    const auto section = projectFile.environment.find(sectionName);
    if (section == projectFile.environment.end()) return;

    section->second.insert_or_assign(variableName, value);
}

void ProjectFileHandler::removeVariable(const std::string &sectionName, const std::string &variableName) const {
    const auto section = projectFile.environment.find(sectionName);
    if (section == projectFile.environment.end()) return;

    section->second.erase(variableName);
}

void ProjectFileHandler::buildProjectFile() const {
    std::string contents = "; VoxelFox project\n";

    for (const auto& [sectionName, variables] : projectFile.environment) {
        contents += std::format("[{}]\n", sectionName);

        for (const auto& [variableName, value] : variables) {
            contents += std::format(
                "{}={}\n",
                variableName,
                valueToString(value)
            );
        }

        contents += '\n';
    }

    projectFile.file.writeText(contents);
}

void ProjectFileHandler::save() const {
    buildProjectFile();
}
