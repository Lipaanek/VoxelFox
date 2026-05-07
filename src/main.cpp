#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "headers/shader_loader.hpp"
#include "headers/vertex.hpp"
#include "headers/mesh.hpp"
#include "headers/object_loader.hpp"
#include "headers/camera.hpp"

using namespace std;

float velocityX = 0.0f;
float velocityY = 0.0f;
float velocityZ = 0.0f;

const float speed = 0.05f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void input_fallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        velocityX = -speed;
    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        velocityX = 0.0f;

    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        velocityX = speed;
    if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        velocityX = 0.0f;

    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        velocityY = speed;
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        velocityY = 0.0f;

    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        velocityY = -speed;
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        velocityY = 0.0f;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        velocityZ = speed;
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        velocityZ = 0.0f;

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
        velocityZ = -speed;
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
        velocityZ = 0.0f;
}

void setUniforms(int shaderProgram, Camera camera) {
    glm::mat4 model = glm::mat4(1.0f); // Identity, object at origin
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(800.0f/600.0f);

    // Get & set uniform locations
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Get & Set lighting uniforms
    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.position));
    glUniform3f(lightPosLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 0.8f, 0.5f, 0.2f);
}

void draw(GLFWwindow* window, int vertexCount) {
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glfwSwapBuffers(window);
}

// setup shaders and all that
void predraw(int VAO, int shaderProgram) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glEnable(GL_DEPTH_TEST);
}

// clear screen
void clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main()
{
    if (!glfwInit()) {
        printf("%s", "Failed to init a window");

        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "VoxelFox", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to open GLFW window");
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    loadObject("src/meshes/test.obj");
    vector<Vertex> vertices = getWorldData();

    Mesh mesh = createMesh(vertices);

    // both internal shaders are loaded
    string vertexCode = loadFile("src/shaders/vertex.glsl");
    string fragmentCode = loadFile("src/shaders/fragment.glsl");

    // compile them both
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    // shader where I bind all the shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glfwSetKeyCallback(window, input_fallback);

    Camera camera;
    camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
    camera.updateCameraVectors();

    // main loop
    while(!glfwWindowShouldClose(window))
    {
        clear();
        predraw(mesh.VAO, shaderProgram);
        camera.updateCameraPosition(velocityX, velocityY, velocityZ);
        setUniforms(shaderProgram, camera);

        draw(window, mesh.vertexCount);
        glfwPollEvents();   
    }

    glfwTerminate();
    return 0;
}