#ifndef PROPERTY_SYSTEM_HPP
#define PROPERTY_SYSTEM_HPP

// Basic types, can be extended later on
enum class PropertyType {
    FLOAT, VEC2, VEC3, VEC4, COLOR, BOOL, STRING, ENUM
};

struct PropertyDescriptor {
    std::string name;
    PropertyType type;
    void* valuePtr; // Pointer to value in node
    union {
        struct { float min, max; } floatRange;
        struct { std::vector<std::string> options; } enumOptions;
    };
    std::string category; // Property grouping
};

#endif