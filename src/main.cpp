#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "openglErrorReporting.h"
#include <stb_image.h>

#include <sys/types.h>
#include <vector>
#include <print>


#define wWIDTH 800
#define wHEIGHT 600

//==================== Shaders and scene elements ====================
const GLchar* vsSource = R"(
#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec3 aColor;
out vec3 fragColor;

void main(){
    fragColor = aColor;
    gl_Position = vec4(aPosition, 0.0f, 1.0f);
}
)";

const GLchar* fsSource = R"(
#version 330 core
in vec3 fragColor;
out vec4 FragColor;

void main(){
    FragColor = vec4(fragColor, 1.0f);
}
)";

const std::vector<GLfloat> triangleVertices = {
    //  X  Y        R   G   B
    0.0f,  0.8f,  1.0f, 0.0f, 0.0f,         // top center
    0.5f, 0.0f,  0.0f, 1.0f, 0.0f,          // center right
    -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,    // center left
    0.0f,  -0.8f,  1.0f, 1.0f, 0.0f    // bottom center   
};

const std::vector<GLuint> indices = {
    0, 1, 2, // first triangle
    2, 1, 3  // second triangle
};

//==================== Helpers and callbacks ====================

void framebuffer_size_callback(GLFWwindow* w, int width, int height){
    glViewport(0, 0, width, height);
}

void init_glfw(){
    if(!glfwInit()) {
        std::println("Error initializing glfw");
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void processInputs(GLFWwindow *w){
    // https://www.glfw.org/docs/latest/input_guide.html
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(w, true);
    }
}


int main(){
    //======================= GLFW: Initialization =======================
    init_glfw();

    GLFWwindow *window = glfwCreateWindow(wWIDTH, wHEIGHT, "OpenGL", nullptr, nullptr);
    if (!window) {
        std::println("Error initializing window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load icon
    GLFWimage images[1];
    images[0].pixels = stbi_load(RESOURCES_PATH"triangle-64.png", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    //=============== GLAD: load all OpenGL function pointers ===============
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::println("Failed to initialize GLAD");
        return -1;
    }

    // Enable opengl debug output
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT){
        enableReportGlErrors();
    }

    //======================= OpenGL global configs =======================
    glViewport(0, 0, wWIDTH, wHEIGHT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //GL_LINE - wireframe

    //==================== Create and compile shaders ====================
    GLint success;
    char infoLogBuf[512];

    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLogBuf);
        std::println("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLogBuf);
        return -1;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLogBuf);
        std::println("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", infoLogBuf);
        return -1;
    }

    // Create program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success){
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLogBuf);
        std::println("ERROR::PROGRAM::LINKING_FAILED\n{}", infoLogBuf);
        return -1;
    }

    // Check whether the program is healthy
    glValidateProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
    if (!success){
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLogBuf);
        std::println("ERROR::PROGRAM::VALIDATION_FAILED\n{}", infoLogBuf);
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //========= set up vertex data (and buffer(s)) and configure vertex attributes =========
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO first then bind buffers to it
    glBindVertexArray(VAO);

    // VertexBufferObject
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        triangleVertices.size() * sizeof(GLfloat),
        triangleVertices.data(),
        GL_STATIC_DRAW);

    // ElementBufferObject
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW);

    // Configure attribute aPosition
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(0);

    // Configure attribute aColor
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(GLfloat),
        (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //=============================== Main loop ===============================
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    while (!glfwWindowShouldClose(window)) {

        // Process inputs
        processInputs(window);

        // Render
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Check and call events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    //=============================== Teardown ===============================
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
}