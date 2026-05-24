#ifndef ECS_HPP
#define ECS_HPP

#include "node.hpp"
#include "components.hpp"
#include "system.hpp"
#include <unordered_map>

using Entity = uint32_t;
constexpr Entity INVALID_ENTITY = 0;

// Boilerplate code for component storage
template<typename T>
class ComponentStorage {
    public:
        std::vector<Entity> entities;
        std::vector<T> components;
        std::unordered_map<Entity, size_t> indexMap;

        void Add(Entity e, const T& component) {
            // Avoid duplicates
            if (indexMap.find(e) != indexMap.end())
                return;

                size_t index = components.size();

                entities.push_back(e);
                components.push_back(component);
                indexMap[e] = index;
        }

        T* Get(Entity e) {
            auto it = indexMap.find(e);
            if (it ==  indexMap.end())
                return nullptr;

            return &components[it->second];
        }

        void Remove(Entity e) {
            auto it = indexMap.find(e);
            if (it == indexMap.end())
                return;

            size_t index = it->second;
            size_t lastIndex = components.size() - 1;

            // Swap with last element
            components[index] = components[lastIndex];
            entities[index] = entities[lastIndex];

            indexMap[entities[index]] = index;

            components.pop_back();
            entities.pop_back();
            indexMap.erase(e);
        }
};

class SystemManager {
    public:
        std::vector<System*> systems;

        void Add(System* s) {
            systems.push_back(s);
        }

        void UpdateAll(float dt) {
            for (System* s : systems)
                s->Update(dt);
        }
};

// For every component add
// A new ComponentStorage<T>,
// So the engine can work
// With that later on
class ECS {
    public:
        ComponentStorage<Transform> transforms;
};

#endif