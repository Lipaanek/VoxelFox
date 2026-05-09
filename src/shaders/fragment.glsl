#version 430 core
// Outputs
out vec4 FragColor;

// Inputs
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); // Normalized vector pointing to the light
    float diff = max(dot(norm, lightDir), 0.0); // Lambert (Diffuse) lighting
    vec3 diffuse = diff * lightColor; // Apply light color
    
    vec3 result = (diffuse + 0.1) * objectColor;  // + 0.1 ambient
    FragColor = vec4(result, 1.0); // Apply color
}