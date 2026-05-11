#version 430 core

layout(local_size_x = 1) in;

struct PrecomputedTriangle {
    vec3 v0;
    float _pad0;
    vec3 v1;
    float _pad1;
    vec3 v2;
    float _pad2;
    vec3 e0;
    float _pad3;
    vec3 e1;
    float _pad4;
    vec3 e2;
    float _pad5;
    vec3 normal;
    float d;
    ivec3 voxelMin;
    float _pad_voxelMin;
    ivec3 voxelMax;
    int materialIndex;
};

layout(std430, binding = 0) readonly buffer TriangleBuffer {
    PrecomputedTriangle triangles[];
};

layout(std430, binding = 1) buffer VoxelGrid {
    uint voxels[];
};

uniform float voxelSize;
uniform vec3 gridOrigin;
uniform ivec3 gridSize;
uniform uint baseIndex;

shared PrecomputedTriangle sharedTri;

// Helper function to see, if overlaps on the X axis
bool axisTestX(vec3 e, vec3 v0, vec3 v1, vec3 v2, vec3 boxHalf) {
    float a = e.z * v0.y - e.y * v0.z;
    float b = e.z * v1.y - e.y * v1.z;
    float c = e.z * v2.y - e.y * v2.z;
    float mn = min(min(a, b), c);
    float mx = max(max(a, b), c);
    float r = boxHalf.y * abs(e.z) + boxHalf.z * abs(e.y);
    return !(mn > r || mx < -r);
}

// Helper function to see, if overlaps on the Y axis
bool axisTestY(vec3 e, vec3 v0, vec3 v1, vec3 v2, vec3 boxHalf) {
    float a = -e.z * v0.x + e.x * v0.z;
    float b = -e.z * v1.x + e.x * v1.z;
    float c = -e.z * v2.x + e.x * v2.z;
    float mn = min(min(a, b), c);
    float mx = max(max(a, b), c);
    float r = boxHalf.x * abs(e.z) + boxHalf.z * abs(e.x);
    return !(mn > r || mx < -r);
}

// Helper function to see, if overlaps on the Z axis
bool axisTestZ(vec3 e, vec3 v0, vec3 v1, vec3 v2, vec3 boxHalf) {
    float a = e.y * v0.x - e.x * v0.y;
    float b = e.y * v1.x - e.x * v1.y;
    float c = e.y * v2.x - e.x * v2.y;
    float mn = min(min(a, b), c);
    float mx = max(max(a, b), c);
    float r = boxHalf.x * abs(e.y) + boxHalf.y * abs(e.x);
    return !(mn > r || mx < -r);
}

// Akenine-Möller's triangle-box overlap test 
bool mollerTriBoxOverlap(PrecomputedTriangle tri, vec3 boxCenter, vec3 boxHalf) {
    vec3 tv0 = tri.v0 - boxCenter;
    vec3 tv1 = tri.v1 - boxCenter;
    vec3 tv2 = tri.v2 - boxCenter;

    float mn, mx;

    mn = min(min(tv0.x, tv1.x), tv2.x);
    mx = max(max(tv0.x, tv1.x), tv2.x);
    if (mn > boxHalf.x || mx < -boxHalf.x) return false;

    mn = min(min(tv0.y, tv1.y), tv2.y);
    mx = max(max(tv0.y, tv1.y), tv2.y);
    if (mn > boxHalf.y || mx < -boxHalf.y) return false;

    mn = min(min(tv0.z, tv1.z), tv2.z);
    mx = max(max(tv0.z, tv1.z), tv2.z);
    if (mn > boxHalf.z || mx < -boxHalf.z) return false;

    float v0n = dot(tv0, tri.normal);
    float v1n = dot(tv1, tri.normal);
    float v2n = dot(tv2, tri.normal);
    float rn = dot(boxHalf, abs(tri.normal));
    mn = min(min(v0n, v1n), v2n);
    mx = max(max(v0n, v1n), v2n);
    if (mn > rn || mx < -rn) return false;

    if (!axisTestX(tri.e0, tv0, tv1, tv2, boxHalf)) return false;
    if (!axisTestY(tri.e0, tv0, tv1, tv2, boxHalf)) return false;
    if (!axisTestZ(tri.e0, tv0, tv1, tv2, boxHalf)) return false;

    if (!axisTestX(tri.e1, tv0, tv1, tv2, boxHalf)) return false;
    if (!axisTestY(tri.e1, tv0, tv1, tv2, boxHalf)) return false;
    if (!axisTestZ(tri.e1, tv0, tv1, tv2, boxHalf)) return false;

    if (!axisTestX(tri.e2, tv0, tv1, tv2, boxHalf)) return false;
    if (!axisTestY(tri.e2, tv0, tv1, tv2, boxHalf)) return false;
    if (!axisTestZ(tri.e2, tv0, tv1, tv2, boxHalf)) return false;

    return true;
}

void main() {
    uint triIdx = gl_GlobalInvocationID.x + baseIndex;
    if (triIdx >= triangles.length()) return;

    if (gl_LocalInvocationIndex == 0) {
        sharedTri = triangles[triIdx];
    }
    barrier();

    PrecomputedTriangle tri = sharedTri;

    ivec3 mn = tri.voxelMin;
    ivec3 mx = tri.voxelMax;
    ivec3 extent = mx - mn + 1;
    uint total = extent.x * extent.y * extent.z;

    vec3 boxHalf = vec3(voxelSize * 0.5);

    uint strideXY = uint(gridSize.x) * uint(gridSize.y);

    // Voxelization itself
    for (uint i = gl_LocalInvocationIndex; i < total; i += gl_WorkGroupSize.x) {
        uint vx = uint(mn.x) + (i % uint(extent.x));
        uint vy = uint(mn.y) + ((i / uint(extent.x)) % uint(extent.y));
        uint vz = uint(mn.z) + (i / (uint(extent.x) * uint(extent.y)));

        if (vx >= uint(gridSize.x) || vy >= uint(gridSize.y) || vz >= uint(gridSize.z)) continue;

        uint idx = vz * strideXY + vy * uint(gridSize.x) + vx;

        vec3 center = gridOrigin + (vec3(vx, vy, vz) + 0.5) * voxelSize;

        if (mollerTriBoxOverlap(tri, center, boxHalf)) {
            uint matVal = 1u | (uint(tri.materialIndex) << 2u);
            atomicMax(voxels[idx], matVal);
        }
    }
}
