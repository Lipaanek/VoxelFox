#include "util.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace Util {
namespace File {

    std::string read(const std::string& path) {
        std::ifstream file(path);

        if (!file.is_open())
            return "";

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }
}

namespace Log {

    void error(const std::string& message) {
        std::cerr << "[ERROR] " << message << "\n";
    }

    void log(const std::string& message) {
        std::cout << "[LOG] " << message << "\n";
    }

    void scriptLoadLog(const LoadScriptResult& result) {
        switch (result.result) {
            case LoadResult::Success:
                break;

            case LoadResult::Skipped:
                Util::Log::log(result.message);
                break;

            case LoadResult::Failed:
                Util::Log::error(result.message);
                break;
        }
    }
}

namespace String {

    std::vector<std::string> split(std::string s, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        tokens.push_back(s);

        return tokens;
    }

    std::vector<std::string> splitWhitespace(const std::string& s) {
        std::vector<std::string> tokens;
        std::string token;
        for (char c : s) {
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty())
            tokens.push_back(token);

        return tokens;
    }
}
}