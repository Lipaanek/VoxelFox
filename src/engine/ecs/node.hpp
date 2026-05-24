#ifndef NODE_HPP
#define NODE_HPP

#include "node_type.hpp"

using Entity = uint32_t;
constexpr Entity INVALID_ENTITY = 0;

class Node {
    public:
        std::string name;
        Node* parent = nullptr;
        std::vector<std::unique_ptr<Node>> children;

        Entity id = INVALID_ENTITY;
        
        const std:string& GetName() const {
            return name;
        }
        
        void AddChildren(Node* child) {
            child->parent = this;
            children.push_back(child);
        }

        virtual ~Node() = default;
};

#endif