#version 430 core
// Outputs
out vec4 FragColor;

// Inputs
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 VertexColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

// Basic lighting
// TODO: Replace with raycasting
void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 result = (diffuse + 0.1) * VertexColor;
    FragColor = vec4(result, 1.0);
}