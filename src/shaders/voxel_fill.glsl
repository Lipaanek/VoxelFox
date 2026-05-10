#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(std430, binding = 1) buffer VoxelGrid {
    uint voxels[];
};

uniform ivec3 gridSize;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= gridSize.x || pos.y >= gridSize.y) return;

    uint strideY = uint(gridSize.x);
    uint strideZ = uint(gridSize.x) * uint(gridSize.y);

    bool inside = false;
    bool wasSurface = false;

    for (int z = 0; z < gridSize.z; z++) {
        uint idx = uint(z) * strideZ + uint(pos.y) * strideY + uint(pos.x);
        uint v = voxels[idx];
        bool isSurface = (v == 1u);

        if (isSurface && !wasSurface) {
            inside = !inside;
        }

        if (inside && !isSurface) {
            voxels[idx] = 2u;
        }

        wasSurface = isSurface;
    }
}
