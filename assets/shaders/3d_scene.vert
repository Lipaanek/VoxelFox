#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

uniform mat4 u_view;
uniform mat4 u_projection;

struct Instance {
    mat4 model;
    vec4 color;
};

layout (std430, binding = 0) readonly buffer InstanceBuffer {
    Instance instances[];
};

out vec3 vColor;
out vec3 vWorldPos;
out vec3 vNormal;

void main() {
    Instance instance = instances[gl_InstanceID];

    mat4 model = instance.model;

    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = u_projection * u_view * worldPos;

    vWorldPos = worldPos.xyz;
    vNormal = normalize(transpose(inverse(mat3(model))) * aNormal);
    vColor = aColor * instance.color.rgb;
}
