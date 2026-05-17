#ifndef CHUNK_HPP
#define CHUNK_HPP

struct ChunkKey {
    int x;
    int y;
    int z;

    bool operator==(const ChunkKey&) const = default;
};

struct ChunkKeyHash {
    size_t operator()(const ChunkKey& k) const {
        return
            ((size_t)k.x * 73856093) ^
            ((size_t)k.y * 19349663) ^
            ((size_t)k.z * 83492791);
    }
};

#endif