#pragma once
#include <string>
#include <vector>

class LuaFlags {
private:
    std::vector<std::string> m_flags_;

public:
    void parse(const std::string& src);
    void clear();
    [[nodiscard]] bool has(const std::string& flag) const;
    [[nodiscard]] const std::vector<std::string>& all() const;
};