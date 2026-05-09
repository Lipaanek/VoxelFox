#version 430 core

layout(local_size_x = 64) in;

struct Triangle {
    vec3 p0;
    vec3 p1;
    vec3 p2;
};

layout(std430, binding = 0) restrict readonly buffer TriangleBuffer {
    Triangle triangles[];
};

layout(std430, binding = 1) restrict readonly buffer TriangleIndices {
    uint triIndices[];
};

layout(std430, binding = 2) restrict writeonly buffer VoxelOutput {
    uint solidVoxels[];
};

uniform uint triIndexCount;
uniform uint chunkSize;
uniform float voxelSize;
uniform vec3 chunkOffset;

vec3 computeTriangleNormal(Triangle t) {
    vec3 edge1 = t.p1 - t.p0;
    vec3 edge2 = t.p2 - t.p0;
    vec3 n = cross(edge1, edge2);
    return length(n) < 0.0001 ? vec3(0, 1, 0) : normalize(n);
}

bool pointInTriangle(vec3 p, Triangle t) {
    vec3 v0 = t.p1 - t.p0;
    vec3 v1 = t.p2 - t.p0;
    vec3 v2 = p - t.p0;

    float dot00 = dot(v0, v0);
    float dot01 = dot(v0, v1);
    float dot02 = dot(v0, v2);
    float dot11 = dot(v1, v1);
    float dot12 = dot(v1, v2);

    float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0.0) && (v >= 0.0) && (u + v <= 1.0);
}

bool voxelIntersectsTriangle(vec3 voxelMin, vec3 voxelMax, Triangle t) {
    vec3 center = (voxelMin + voxelMax) * 0.5;
    vec3 halfExtents = (voxelMax - voxelMin) * 0.5;

    vec3 normal = computeTriangleNormal(t);
    vec3 axis1 = normalize(normal);
    vec3 crossAxis = cross(axis1, vec3(1, 0, 0));
    vec3 axis2 = length(crossAxis) < 0.0001 ? vec3(0, 0, 1) : normalize(crossAxis);

    vec3 axes[5];
    axes[0] = vec3(1, 0, 0);
    axes[1] = vec3(0, 1, 0);
    axes[2] = vec3(0, 0, 1);
    axes[3] = axis1;
    axes[4] = axis2;

    for (int i = 0; i < 5; i++) {
        vec3 axis = normalize(axes[i]);
        if (length(axis) < 0.0001) continue;

        float projVoxelMin = dot(center, axis) - dot(halfExtents, abs(axis));
        float projVoxelMax = dot(center, axis) + dot(halfExtents, abs(axis));

        float projTriMin = min(min(dot(t.p0, axis), dot(t.p1, axis)), dot(t.p2, axis));
        float projTriMax = max(max(dot(t.p0, axis), dot(t.p1, axis)), dot(t.p2, axis));

        if (projVoxelMax < projTriMin || projTriMax < projVoxelMin) {
            return false;
        }
    }

    vec3 corners[8];
    corners[0] = voxelMin;
    corners[1] = vec3(voxelMax.x, voxelMin.y, voxelMin.z);
    corners[2] = vec3(voxelMax.x, voxelMax.y, voxelMin.z);
    corners[3] = vec3(voxelMin.x, voxelMax.y, voxelMin.z);
    corners[4] = vec3(voxelMin.x, voxelMin.y, voxelMax.z);
    corners[5] = vec3(voxelMax.x, voxelMin.y, voxelMax.z);
    corners[6] = voxelMax;
    corners[7] = vec3(voxelMin.x, voxelMax.y, voxelMax.z);

    for (int j = 0; j < 8; j++) {
        if (pointInTriangle(corners[j], t)) {
            return true;
        }
    }

    if ((t.p0.x >= voxelMin.x && t.p0.x <= voxelMax.x &&
         t.p0.y >= voxelMin.y && t.p0.y <= voxelMax.y &&
         t.p0.z >= voxelMin.z && t.p0.z <= voxelMax.z) ||
        (t.p1.x >= voxelMin.x && t.p1.x <= voxelMax.x &&
         t.p1.y >= voxelMin.y && t.p1.y <= voxelMax.y &&
         t.p1.z >= voxelMin.z && t.p1.z <= voxelMax.z) ||
        (t.p2.x >= voxelMin.x && t.p2.x <= voxelMax.x &&
         t.p2.y >= voxelMin.y && t.p2.y <= voxelMax.y &&
         t.p2.z >= voxelMin.z && t.p2.z <= voxelMax.z)) {
        return true;
    }

    return false;
}

void main() {
    uint globalIdx = gl_GlobalInvocationID.x;
    uint voxelsPerChunk = chunkSize * chunkSize * chunkSize;

    if (globalIdx >= voxelsPerChunk) return;

    uint x = globalIdx % chunkSize;
    uint y = (globalIdx / chunkSize) % chunkSize;
    uint z = globalIdx / (chunkSize * chunkSize);

    vec3 voxelMin = chunkOffset + vec3(x, y, z) * voxelSize;
    vec3 voxelMax = voxelMin + vec3(voxelSize);

    for (uint i = 0; i < triIndexCount; i++) {
        uint idx = triIndices[i];
        Triangle t = triangles[idx];
        if (voxelIntersectsTriangle(voxelMin, voxelMax, t)) {
            solidVoxels[globalIdx] = 1u;
            return;
        }
    }

    solidVoxels[globalIdx] = 0u;
}