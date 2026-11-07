#pragma once

#include <memory>
#include <unordered_map>
#include <type_traits>

#include "resource.hpp"
#include "../util/util.hpp"

class ResourceManager {
public:
    template<typename T>
    std::shared_ptr<T> load(const ResourceLoadInfo& loadInfo) {
        static_assert(std::is_base_of_v<Resource, T>);

        auto resource = std::make_shared<T>();
        if (!resource->loadFromFile(loadInfo)) {
            Util::Log::error("ResourceManager: failed to load resource");
            return {};
        }

        resource->id = nextID_++;
        cache.emplace(resource->id, resource);
        return resource;
    }

    void unload(ResourceID id);

    template<typename T>
    std::shared_ptr<T> getResource(ResourceID id) const {
        static_assert(std::is_base_of_v<Resource, T>);

        const auto it = cache.find(id);
        if (it == cache.end())
            return {};

        return std::dynamic_pointer_cast<T>(it->second);
    }

private:
    std::unordered_map<ResourceID, std::shared_ptr<Resource>> cache;
    ResourceID nextID_ = 1;
};
