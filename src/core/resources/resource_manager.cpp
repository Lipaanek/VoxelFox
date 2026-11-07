#include "resource_manager.hpp"

void ResourceManager::unload(ResourceID id) {
    cache.erase(id);
}