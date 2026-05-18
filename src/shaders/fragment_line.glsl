#version 430 core
out vec4 FragColor;

in vec3 VertexColor;
in vec3 FragPos;

uniform vec3 viewPos;
uniform float fadeNear;
uniform float fadeFar;

void main() {
    float camDist = distance(FragPos, viewPos);
    float t = clamp((camDist - fadeNear) / (fadeFar - fadeNear), 0.0, 1.0);
    t = smoothstep(0.0, 1.0, t);
    float alpha = 1.0 - t;
    FragColor = vec4(VertexColor, alpha);
}
