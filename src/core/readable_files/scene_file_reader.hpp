#pragma once

#include <string_view>
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>

struct SceneFileValue {
    using Array = std::vector<SceneFileValue>;

    std::variant<
        std::string,
        bool,
        double,
        Array
    > value;
};

struct SceneSection {
    std::string name;
    std::unordered_map<std::string, SceneFileValue> properties;
};

using SceneFile = std::vector<SceneSection>;


//Example syntax
//```text
//[node]
//name = "Cube"
//type = "MeshInstance3D"
//mesh = Resource("res://models/cube.mesh")
//```
class SceneFileReader {
private:
    std::string m_sourceCode {};
    size_t m_currentIndex { 0 };

    void whitespace();
    char advance();
    [[nodiscard]] char current() const;
    [[nodiscard]] char peek() const;
    [[nodiscard]] bool isAtEnd() const;

    bool match(char c);
    void consume(char c, std::string_view errorMsg);

    [[noreturn]] void error(std::string_view message) const;

    SceneSection parseSection();
    void parseProperty(SceneSection& section);

    SceneFileValue parseValue();

    std::string parseString();
    double parseNumber();
    bool parseBoolean();
    SceneFileValue::Array parseArray();

    std::string parseIdentifier();

public:
    explicit SceneFileReader(std::string_view source);

    SceneFile parse();
};