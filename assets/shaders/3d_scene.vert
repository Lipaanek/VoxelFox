#version 430 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in vec3 a_color;

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

    vec4 worldPos = model * vec4(a_pos, 1.0);
    gl_Position = u_projection * u_view * worldPos;

    vWorldPos = worldPos.xyz;
    vNormal = normalize(transpose(inverse(mat3(model))) * a_normal);
    vColor = a_color * instance.color.rgb;
}
