#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "headers/shader_loader.hpp"
#include "headers/vertex.hpp"
#include <glm/glm.hpp>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void draw(GLFWwindow* window) {
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
}

// setup shaders and all that
void predraw(int VAO, int shaderProgram) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
}

// clear screen
void clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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

    Vertex vertices[] = {
        {
            {-0.5f, -0.5f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f}
        },
        {
            {0.5f, -0.5f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f}
        },
        {
            {0.0f, 0.5f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {0.5f, 1.0f}
        }
    };

    unsigned int VAO; // Vertex Array Obj - memory w vertecies
    unsigned int VBO; // Vertex Buffer Obj - instructions for reading vertex data

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // load vertex data onto memory

    //Attr Location, Read 3 Floats, Datatype, tight packing, offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // both internal files loaded
    std::string vertexCode = loadFile("src/shaders/vertex.glsl");
    std::string fragmentCode = loadFile("src/shaders/fragment.glsl");

    // compile them both
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    // shader where I bind all the shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // main loop
    while(!glfwWindowShouldClose(window))
    {
        clear();
        predraw(VAO, shaderProgram);
        draw(window);
        glfwPollEvents();   
    }

    glfwTerminate();
    return 0;
}