#version 430 core
layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct Vertex {
    vec4 position; // Use vec4 for alignment (w is ignored)
    vec4 normal;   
    vec2 UV;
    vec2 _padding; // Keep the whole struct a multiple of 16
};

layout(std430, binding = 0) buffer MeshBuffer {
    Vertex vertices[];
};

layout(std430, binding = 1) buffer VoxelOutput {
    uint voxelFlags[];
};

uniform uint vertexCount;
uniform float voxelSize;
uniform vec3 boundsMin;
uniform vec3 boundsMax;
uniform int gridSizeX;
uniform int gridSizeY;
uniform int gridSizeZ;

bool pointInTriangleBarycentric(vec3 p, vec3 a, vec3 b, vec3 c) {
    vec3 v0 = b - a;
    vec3 v1 = c - a;
    vec3 v2 = p - a;
    
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    
    float denom = d00 * d11 - d01 * d01;
    if (abs(denom) < 1e-10) return false;
    
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0 - v - w;
    
    return u >= 0.0 && v >= 0.0 && w >= 0.0;
}

bool triangleVoxelOverlap(vec3 a, vec3 b, vec3 c, vec3 voxelMin, vec3 voxelMax) {
    vec3 corners[8] = {
        voxelMin,
        vec3(voxelMax.x, voxelMin.y, voxelMin.z),
        vec3(voxelMin.x, voxelMax.y, voxelMin.z),
        vec3(voxelMin.x, voxelMin.y, voxelMax.z),
        vec3(voxelMax.x, voxelMax.y, voxelMin.z),
        vec3(voxelMax.x, voxelMin.y, voxelMax.z),
        vec3(voxelMin.x, voxelMax.y, voxelMax.z),
        voxelMax
    };
    
    for (int i = 0; i < 8; i++) {
        if (pointInTriangleBarycentric(corners[i], a, b, c)) {
            return true;
        }
    }
    
    for (int i = 0; i < 8; i++) {
        vec3 p = corners[i];
        if (pointInTriangleBarycentric(p + vec3(voxelSize,0,0), a, b, c) ||
            pointInTriangleBarycentric(p - vec3(voxelSize,0,0), a, b, c) ||
            pointInTriangleBarycentric(p + vec3(0,voxelSize,0), a, b, c) ||
            pointInTriangleBarycentric(p - vec3(0,voxelSize,0), a, b, c) ||
            pointInTriangleBarycentric(p + vec3(0,0,voxelSize), a, b, c) ||
            pointInTriangleBarycentric(p - vec3(0,0,voxelSize), a, b, c)) {
            return true;
        }
    }
    
    return false;
}

void main() {
    uint triIdx = gl_GlobalInvocationID.x;
    
    if (triIdx * 3 + 2 >= vertexCount) return;
    
    Vertex v0 = vertices[triIdx * 3];
    Vertex v1 = vertices[triIdx * 3 + 1];
    Vertex v2 = vertices[triIdx * 3 + 2];
    
    vec3 a = v0.position.xyz;
    vec3 b = v1.position.xyz;
    vec3 c = v2.position.xyz;
    
    vec3 triMin = min(a, min(b, c));
    vec3 triMax = max(a, max(b, c));
    
    ivec3 minVoxel = ivec3(floor((triMin.x - voxelSize * 0.5 - boundsMin.x) / voxelSize),
                          floor((triMin.y - voxelSize * 0.5 - boundsMin.y) / voxelSize),
                          floor((triMin.z - voxelSize * 0.5 - boundsMin.z) / voxelSize));
    ivec3 maxVoxel = ivec3(floor((triMax.x + voxelSize * 0.5 - boundsMin.x) / voxelSize),
                          floor((triMax.y + voxelSize * 0.5 - boundsMin.y) / voxelSize),
                          floor((triMax.z + voxelSize * 0.5 - boundsMin.z) / voxelSize));
    
    minVoxel = max(minVoxel, ivec3(0));
    maxVoxel = min(maxVoxel, ivec3(gridSizeX - 1, gridSizeY - 1, gridSizeZ - 1));
    
    for (int z = minVoxel.z; z <= maxVoxel.z; z++) {
        for (int y = minVoxel.y; y <= maxVoxel.y; y++) {
            for (int x = minVoxel.x; x <= maxVoxel.x; x++) {
                vec3 voxelMin = boundsMin + vec3(x, y, z) * voxelSize;
                vec3 voxelMax = voxelMin + vec3(voxelSize);
                
                if (triangleVoxelOverlap(a, b, c, voxelMin, voxelMax)) {
                    int idx = z * gridSizeX * gridSizeY + y * gridSizeX + x;
                    atomicOr(voxelFlags[idx], 1u);
                }
            }
        }
    }
}