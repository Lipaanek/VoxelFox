
#include "scene_file_reader.hpp"

#include <cctype>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <utility>

SceneFileReader::SceneFileReader(std::string_view source)
    : m_sourceCode(source) {
}

// --------------------------------------------------
// Character handling
// --------------------------------------------------

bool SceneFileReader::isAtEnd() const {
    return m_currentIndex >= m_sourceCode.size();
}

char SceneFileReader::current() const {
    if (isAtEnd())
        return '\0';

    return m_sourceCode[m_currentIndex];
}

char SceneFileReader::peek() const {
    if (m_currentIndex + 1 >= m_sourceCode.size())
        return '\0';

    return m_sourceCode[m_currentIndex + 1];
}

char SceneFileReader::advance() {
    if (isAtEnd())
        return '\0';

    return m_sourceCode[m_currentIndex++];
}

bool SceneFileReader::match(const char c) {
    if (isAtEnd() || current() != c)
        return false;

    advance();
    return true;
}

void SceneFileReader::consume(const char c, const std::string_view errorMsg) {
    if (!match(c))
        error(errorMsg);
}

void SceneFileReader::whitespace() {
    while (!isAtEnd()) {
        if (std::isspace(static_cast<unsigned char>(current()))) {
            advance();
            continue;
        }

        // Comments start with #
        if (current() == '#') {
            while (!isAtEnd() && current() != '\n')
                advance();

            continue;
        }

        break;
    }
}

// --------------------------------------------------
// Error handling
// --------------------------------------------------

[[noreturn]] void SceneFileReader::error(const std::string_view message) const {
    size_t line = 1;
    size_t column = 1;

    for (size_t i = 0; i < m_currentIndex; ++i) {
        if (m_sourceCode[i] == '\n') {
            ++line;
            column = 1;
        }
        else {
            ++column;
        }
    }

    throw std::runtime_error(
        std::string(message) +
        " at line " + std::to_string(line) +
        ", column " + std::to_string(column)
    );
}

// --------------------------------------------------
// Identifiers
// --------------------------------------------------

std::string SceneFileReader::parseIdentifier() {
    std::string identifier;

    while (!isAtEnd()) {
        const char c = current();

        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_')
            break;

        identifier += advance();
    }

    if (identifier.empty())
        error("Expected identifier");

    return identifier;
}

// --------------------------------------------------
// Strings
// --------------------------------------------------

std::string SceneFileReader::parseString() {
    consume('"', "Expected opening quotation mark");

    std::string output;

    while (!isAtEnd()) {
        const char c = advance();

        if (c == '"')
            return output;

        if (c == '\n')
            error("Unclosed string");

        if (c == '\\') {
            if (isAtEnd())
                error("Unclosed escape sequence");

            switch (advance()) {
                case '"':
                    output += '"';
                    break;

                case '\\':
                    output += '\\';
                    break;

                case 'n':
                    output += '\n';
                    break;

                case 't':
                    output += '\t';
                    break;

                case 'r':
                    output += '\r';
                    break;

                default:
                    error("Invalid escape sequence");
            }

            continue;
        }

        output += c;
    }

    error("Unclosed string");
}

// --------------------------------------------------
// Numbers
// --------------------------------------------------

double SceneFileReader::parseNumber() {
    const size_t start = m_currentIndex;

    match('-');

    if (!std::isdigit(static_cast<unsigned char>(current())))
        error("Expected digit");

    if (current() == '0') {
        advance();

        if (std::isdigit(static_cast<unsigned char>(current())))
            error("Leading zeros are not allowed");
    }
    else {
        while (std::isdigit(static_cast<unsigned char>(current())))
            advance();
    }

    if (match('.')) {
        if (!std::isdigit(static_cast<unsigned char>(current())))
            error("Expected digit after decimal point");

        while (std::isdigit(static_cast<unsigned char>(current())))
            advance();
    }

    if (match('e') || match('E')) {
        if (!match('+'))
            match('-');

        if (!std::isdigit(static_cast<unsigned char>(current())))
            error("Expected exponent");

        while (std::isdigit(static_cast<unsigned char>(current())))
            advance();
    }

    const std::string number = m_sourceCode.substr(
        start,
        m_currentIndex - start
    );

    char* end = nullptr;
    const double result = std::strtod(number.c_str(), &end);

    if (end != number.c_str() + number.size() || !std::isfinite(result))
        error("Invalid number");

    return result;
}

// --------------------------------------------------
// Booleans
// --------------------------------------------------

bool SceneFileReader::parseBoolean() {
    const std::string identifier = parseIdentifier();

    if (identifier == "true")
        return true;

    if (identifier == "false")
        return false;

    error("Expected boolean");
}

// --------------------------------------------------
// Arrays
// --------------------------------------------------

SceneFileValue::Array SceneFileReader::parseArray() {
    SceneFileValue::Array output;

    consume('[', "Expected '['");

    whitespace();

    if (match(']'))
        return output;

    while (!isAtEnd()) {
        output.push_back(parseValue());

        whitespace();

        if (match(']'))
            return output;

        consume(',', "Expected ',' between array elements");

        whitespace();

        if (current() == ']')
            error("Trailing comma is not allowed");
    }

    error("Unclosed array");
}

// --------------------------------------------------
// Values
// --------------------------------------------------

SceneFileValue SceneFileReader::parseValue() {
    whitespace();

    switch (current()) {
        case '"':
            return { parseString() };

        case '[':
            return { parseArray() };

        default:
            break;
    }

    if (current() == '-' ||
        std::isdigit(static_cast<unsigned char>(current()))) {
        return { parseNumber() };
    }

    if (std::isalpha(static_cast<unsigned char>(current())) ||
        current() == '_') {
        return { parseBoolean() };
    }

    error("Unexpected value");
}

// --------------------------------------------------
// Properties
// --------------------------------------------------

void SceneFileReader::parseProperty(SceneSection& section) {
    whitespace();

    std::string key = parseIdentifier();

    whitespace();

    consume('=', "Expected '=' after property name");

    whitespace();

    SceneFileValue value = parseValue();

    if (!section.properties.emplace(key, std::move(value)).second)
        error("Duplicate property: " + key);
}

// --------------------------------------------------
// Sections
// --------------------------------------------------

SceneSection SceneFileReader::parseSection() {
    SceneSection section;

    consume('[', "Expected '['");

    while (!isAtEnd() && current() != ']') {
        section.name += advance();
    }

    if (section.name.empty())
        error("Section name cannot be empty");

    consume(']', "Expected ']' after section name");

    whitespace();

    while (!isAtEnd() && current() != '[') {
        parseProperty(section);
        whitespace();
    }

    return section;
}

// --------------------------------------------------
// Entry point
// --------------------------------------------------

SceneFile SceneFileReader::parse() {
    SceneFile scene;

    whitespace();

    while (!isAtEnd()) {
        if (current() != '[')
            error("Expected section declaration");

        scene.push_back(parseSection());

        whitespace();
    }

    return scene;
}
