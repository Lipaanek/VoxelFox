#include "resource_manager.hpp"

void ResourceManager::unload(ResourceID id) {
    m_cache.erase(id);
}