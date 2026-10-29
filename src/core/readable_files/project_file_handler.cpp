#include "project_file_handler.hpp"

#include <charconv>
#include <cctype>
#include <format>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "core/util/util.hpp"

namespace {
    std::string_view trim(std::string_view value) {
        while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
            value.remove_prefix(1);
        }
        while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
            value.remove_suffix(1);
        }
        return value;
    }

    std::string_view withoutComment(std::string_view line) {
        bool quoted = false;
        bool escaped = false;
        for (std::size_t i = 0; i < line.size(); ++i) {
            const char character = line[i];
            if (quoted) {
                if (escaped) {
                    escaped = false;
                }
                else if (character == '\\') {
                    escaped = true;
                }
                else if (character == '"') {
                    quoted = false;
                }
            }
            else if (character == '"') {
                quoted = true;
            }
            else if (character == ';') {
                return line.substr(0, i);
            }
        }
        return line;
    }

    std::string parseString(std::string_view value, std::size_t lineNumber) {
        if (value.size() < 2 || value.back() != '"') {
            throw std::runtime_error(std::format("Unterminated string on line {}", lineNumber));
        }

        std::string result;
        result.reserve(value.size() - 2);
        for (std::size_t i = 1; i < value.size() - 1; ++i) {
            const char character = value[i];
            if (character != '\\') {
                result += character;
                continue;
            }

            if (++i >= value.size() - 1) {
                throw std::runtime_error(std::format("Invalid escape on line {}", lineNumber));
            }
            switch (value[i]) {
            case '\\': result += '\\'; break;
            case '"': result += '"'; break;
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            default:
                throw std::runtime_error(std::format("Unknown escape on line {}", lineNumber));
            }
        }
        return result;
    }

    VariableValue parseValue(std::string_view value, std::size_t lineNumber) {
        value = trim(value);
        if (value.empty()) {
            throw std::runtime_error(std::format("Missing value on line {}", lineNumber));
        }

        if (value.front() == '"') {
            return parseString(value, lineNumber);
        }
        if (value == "true") return true;
        if (value == "false") return false;

        int integerValue = 0;
        const auto integerResult = std::from_chars(
            value.data(), value.data() + value.size(), integerValue
        );
        if (integerResult.ec == std::errc{} && integerResult.ptr == value.data() + value.size()) {
            return integerValue;
        }

        float floatValue = 0.0f;
        const auto floatResult = std::from_chars(
            value.data(), value.data() + value.size(), floatValue
        );
        if (floatResult.ec == std::errc{} && floatResult.ptr == value.data() + value.size()) {
            return floatValue;
        }

        return std::string(value);
    }
}

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

ProjectFile &ProjectFileHandler::getEnv() const {
    return this->projectFile;
}

void ProjectFileHandler::load() const {
    const auto& path = projectFile.file.path();
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error(std::format("Cannot open project file: {}", path.string()));
    }

    std::unordered_map<std::string, ProjectSection> environment;
    std::string currentSection;
    std::string line;
    std::size_t lineNumber = 0;

    while (std::getline(input, line)) {
        ++lineNumber;
        const std::string_view content = trim(withoutComment(line));
        if (content.empty()) continue;

        if (content.front() == '[') {
            if (content.size() < 3 || content.back() != ']') {
                throw std::runtime_error(std::format("Invalid section header on line {}", lineNumber));
            }
            const std::string_view section = trim(content.substr(1, content.size() - 2));
            if (section.empty()) {
                throw std::runtime_error(std::format("Empty section name on line {}", lineNumber));
            }
            currentSection = std::string(section);
            environment.try_emplace(currentSection);
            continue;
        }

        if (currentSection.empty()) {
            throw std::runtime_error(std::format("Variable outside a section on line {}", lineNumber));
        }

        const std::size_t separator = content.find('=');
        if (separator == std::string_view::npos) {
            throw std::runtime_error(std::format("Missing '=' on line {}", lineNumber));
        }

        const std::string_view name = trim(content.substr(0, separator));
        if (name.empty()) {
            throw std::runtime_error(std::format("Empty variable name on line {}", lineNumber));
        }

        environment[currentSection].insert_or_assign(
            std::string(name),
            parseValue(content.substr(separator + 1), lineNumber)
        );
    }

    if (input.bad()) {
        throw std::runtime_error(std::format("Failed reading project file: {}", path.string()));
    }
    projectFile.environment = std::move(environment);
}

void ProjectFileHandler::load(const std::filesystem::path& filePath) const {
    ProjectFile pendingProject{ Util::File(filePath), {} };
    ProjectFileHandler pendingHandler{ pendingProject };
    pendingHandler.load();
    projectFile = std::move(pendingProject);
}

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
