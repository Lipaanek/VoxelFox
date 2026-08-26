#version 330 core

in vec2 vUV;
in vec3 vColor;
in float vAlpha;

uniform vec2 uExtents; // half-width, half-height
uniform float uRadius;

out vec4 FragColor;

float roundedBoxSDF(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - r;
}

void main() {
    vec2 p = vUV * uExtents;

    float dist = roundedBoxSDF(p, uExtents, uRadius);
    float alpha = max(vAlpha - smoothstep(0.0, fwidth(dist), dist), 0.0);

    FragColor = vec4(vColor, alpha);
}

