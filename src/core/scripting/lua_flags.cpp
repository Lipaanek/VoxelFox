#include "lua_flags.hpp"

#include <algorithm>
#include <cctype>

namespace {

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string trimLeading(const std::string& line) {
    std::size_t first = 0;
    while (first < line.size() && std::isspace(static_cast<unsigned char>(line[first])))
        ++first;
    return line.substr(first);
}

bool isFlagLine(const std::string& line) {
    return line.rfind("--!", 0) == 0;
}

bool isCommentOrBlank(const std::string& line) {
    return line.empty() || line.rfind("--", 0) == 0;
}

} // namespace

void LuaFlags::parse(const std::string& source) {
    this->clear();

    std::size_t start = 0;
    while (start <= source.size()) {
        std::size_t end = source.find('\n', start);
        std::string line = source.substr(start, end == std::string::npos ? std::string::npos : end - start);
        std::string trimmed = trimLeading(line);

        if (isFlagLine(trimmed)) {
            std::string body = toLower(trimmed.substr(3));
            std::size_t pos = 0;
            while (pos < body.size()) {
                if (isspace(static_cast<unsigned char>(body[pos]))) {
                    ++pos;
                    continue;
                }
                std::size_t flagEnd = pos;
                while (flagEnd < body.size() && !isspace(static_cast<unsigned char>(body[flagEnd])))
                    ++flagEnd;
                this->flags_.push_back(body.substr(pos, flagEnd - pos));
                pos = flagEnd;
            }
        } else if (!isCommentOrBlank(trimmed)) {
            break;
        }

        if (end == std::string::npos)
            break;
        start = end + 1;
    }
}

void LuaFlags::clear() {
    this->flags_.clear();
}

bool LuaFlags::has(const std::string& flag) const {
    return std::find(this->flags_.begin(), this->flags_.end(), toLower(flag))
        != this->flags_.end();
}

const std::vector<std::string>& LuaFlags::all() const {
    return this->flags_;
}