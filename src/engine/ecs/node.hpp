#ifndef NODE_HPP
#define NODE_HPP

#include "propert_system.hpp"
#include "node_type.hpp"

class Node {
    public:
        std::string name;
        Node* parent = nullptr;
        std::vector<Node*> children;

        virtual std::vector<PropertyDescriptor> GetProperties() const = 0;
        virtual NodeType GetType() const = 0;
        virtual const char* GetName() const = 0;
};

#endif