#pragma once

#include <string>
#include <vector>
#include "../scripting/lua_engine.hpp"

namespace Util {
    namespace File {
        std::string read(const std::string& path);
    }
    namespace Log {
        void error(const std::string& message);
        void log(const std::string& message);
        void scriptLoadLog(const LoadScriptResult& result);
    }
    namespace String {
        std::vector<std::string> split(std::string s, const std::string& delimiter);
        std::vector<std::string> splitWhitespace(const std::string& s);
    }
}