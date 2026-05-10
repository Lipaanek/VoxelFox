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
#include "headers/voxelizer.hpp"

using namespace std;

// Variables
const float width = 1920;
const float height = 1080;

const float voxelSize = 0.1f;

float velocityX = 0.0f;
float velocityY = 0.0f;
float velocityZ = 0.0f;

// Camera settings
const float speed = 10.0f;
const float sens = 0.4f;

double prevMouseX = 0;
double prevMouseY = 0;

// Camera stats
float yaw;
float pitch;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    double dx = prevMouseX - xpos;
    double dy = prevMouseY - ypos;

    yaw += (float) dx * sens * -1;
    pitch += (float) dy * sens;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    prevMouseX = xpos;
    prevMouseY = ypos;
}

// Camera inputs
void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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

// Rendering uniform setup
// TODO: Change it, so it matches later with raycasting
void setUniforms(int shaderProgram, Camera camera) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(width/height);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    int objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.position));
    glUniform3f(lightPosLoc, camera.position.x, camera.position.y, camera.position.z);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(objectColorLoc, 0.8f, 0.5f, 0.2f);
}

// Clearing buffers and screen
void clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main() {
    if (!glfwInit()) {
        printf("%s", "Failed to init a window");
        return -1;
    }

    // GLFW window stuff
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "VoxelFox", NULL, NULL);
    if (window == NULL) {
        printf("Failed to open GLFW window");
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        return -1;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ObjectLoader loader;
    if (!loader.load("src/meshes/test.obj")) {
        printf("Failed to load mesh\n");
        return -1;
    }

    std::vector<VoxelChunk> voxelChunks = voxelizeGPUCompute(loader.getVertices(), loader.getIndices(), voxelSize);
    if (voxelChunks.empty()) {
        printf("Voxelization failed\n");
    }
    VoxelMesh voxelMesh = generateVoxelMesh(voxelChunks);
    Mesh voxelRenderMesh(voxelMesh.vertices, voxelMesh.indices);

    // Shader loading
    string vertexCode = loadFile("src/shaders/vertex.glsl");
    string fragmentCode = loadFile("src/shaders/fragment.glsl");

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glfwSetKeyCallback(window, input_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Camera setup
    Camera camera;
    camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
    camera.yaw = 0.0f;
    camera.pitch = 0.0f;
    camera.updateCameraVectors();

    double previousTime = glfwGetTime();
    double previousFrame = glfwGetTime();
    int frameCount = 0;

    // Main update/render loop
    while(!glfwWindowShouldClose(window)) {
        clear();
        glUseProgram(shaderProgram);

        double currentTime = glfwGetTime();

        // Camera updates
        camera.yaw = yaw;
        camera.pitch = pitch;
        camera.updateCameraVectors();
        camera.updateCameraPosition(velocityX, velocityY, velocityZ, currentTime - previousFrame);

        // Rendering
        setUniforms(shaderProgram, camera);

        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.2f, 0.8f, 0.3f);
        voxelRenderMesh.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();

        frameCount++;

        if (currentTime - previousTime >= 1.0) {
            printf("FPS: %d", frameCount);
            frameCount = 0;
            previousTime = currentTime;
        }

        previousFrame = currentTime;
    }

    glfwTerminate();
    printf("Press Enter to exit...\n");
    getchar();
    return 0;
}