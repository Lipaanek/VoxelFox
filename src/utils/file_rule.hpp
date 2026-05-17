#pragma once

#include <string>
#include <vector>
#include <functional>

enum class RuleType
{
    Directory,
    File
};

class FileRule
{
public:
    using ContentGenerator = std::function<std::string()>;

    static FileRule Dir(const std::string& name);
    static FileRule File(
        const std::string& name,
        ContentGenerator generator = nullptr
    );

    FileRule& Children(std::initializer_list<FileRule> children);

    void Build(const std::string& path) const;

private:
    RuleType type;
    std::string name;

    std::vector<FileRule> children;

    ContentGenerator generator;

private:
    FileRule(RuleType type, const std::string& name);
};