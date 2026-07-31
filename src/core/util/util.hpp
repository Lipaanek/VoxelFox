#pragma once

#include <string>

namespace Util {
    namespace File {
        std::string read(const std::string& path);
    }
    namespace Log {
        void error(const std::string& message);
        void log(const std::string& message);
    }
}