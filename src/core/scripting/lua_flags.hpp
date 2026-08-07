#pragma once
#include <string>
#include <vector>

class LuaFlags {
private:
    std::vector<std::string> flags_;

public:
    void parse(const std::string& src);
    void clear();
    bool has(const std::string& flag) const;
    const std::vector<std::string>& all() const;
};