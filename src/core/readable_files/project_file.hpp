#pragma once

#include <string>
#include <variant>
#include <unordered_map>

#include "../util/file_system.hpp"

using VariableValue = std::variant<int, float, std::string, bool>;
using ProjectSection = std::unordered_map<std::string, VariableValue>;

struct ProjectFile {
    Util::File file;
    std::unordered_map<std::string, ProjectSection> environment;
};
