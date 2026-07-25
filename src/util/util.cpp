#include "util.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

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
    }
}