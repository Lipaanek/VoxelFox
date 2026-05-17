#include "file_rule.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

FileRule::FileRule(RuleType type, const std::string& name) : type(type), name(name) {}

FileRule FileRule::Dir(const std::string& name) {
    return FileRule(RuleType::Directory, name);
}

FileRule FileRule::File(
    const std::string& name,
    ContentGenerator generator
) {
    FileRule rule(RuleType::File, name);
    rule.generator = generator;

    return rule;
}

FileRule& FileRule::Children(std::initializer_list<FileRule> newChildren) {
    children = newChildren;
    return *this;
}

void FileRule::Build(const std::string& path) const {
    fs::path current = fs::path(path) / name;

    if (type == RuleType::Directory) {
        fs::create_directories(current);

        for (const auto& child : children) {
            child.Build(current.string());
        }
    }
    else if (type == RuleType::File) {
        std::ofstream file(current);

        if (generator) {
            file << generator();
        }
    }
}