/**
 * Copyright (C) 2018 Bart de Haan
 *
 * polarGB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * polarGB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <stdlib.h>
#include "polarGB/video.h"


const char *vertexShaderSrc = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

const char *fragmentShaderSrc = R"(
    #version 330 core
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)";


Video::Video()
{
    this->window = nullptr;
    this->windowName = "polarGB";
    this->width = 800;
    this->height = 600;
}


Video::~Video()
{
    this->cleanUp();
}


/**
 * Initialise GLFW and GLAD. This initialises OpenGL. We also create a Window on which we can draw
 * graphics.
 */
void Video::initialise()
{
    /**
     * Initialise GLFW for OpenGL. Use version 3.3 with the core profile.
     */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /**
     * Create a window on which we can draw.
     */
    this->window = glfwCreateWindow(this->width, this->height, this->windowName.c_str(), NULL, NULL);
    if (this->window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    /**
     * Load GLAD which is responsible for function pointer for OpenGL and makes everything
     * cross-platform.
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    /**
     * Define a viewport for OpenGL.
     */
    glViewport(0, 0, this->width, this->height);
    glfwSetFramebufferSizeCallback(this->window, this->framebufferSizeCallback);

    /* Setup shaders. */
    initShaders();
}

/**
 * Initialise the shaders used rendering.
 */
void Video::initShaders()
{
    int  success;
    char infoLog[512];
    unsigned int vertexShaderID;
    unsigned int fragmentShaderID;

    /**
     * Vertex shader.
     */
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShaderID);

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    /**
     * Fragment shader.
     */
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShaderID);

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    /**
     * Create shader program for our shaders. (Pipeline the shaders in-and output.)
     */
    this->shaderProgram = glCreateProgram();
    glAttachShader(this->shaderProgram, vertexShaderID);
    glAttachShader(this->shaderProgram, fragmentShaderID);
    glLinkProgram(this->shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glUseProgram(this->shaderProgram);

    /* Clean up. */
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}


/**
 * Destroy the window and clean up GLFW.
 */
void Video::cleanUp()
{
    glfwTerminate();
    this->window = nullptr;
}


/**
 * This function is called when the window is resized. We update the viewport so everything scales
 * with the resize of the window.
 */
void Video::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


/**
 * Update the window.
 */
void Video::update()
{
    processInput(this->window);
    glfwPollEvents();
}


void Video::drawFrame()
{
    float vertices[] =
    {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);

    // 0. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 1. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* Handle input. */
    processInput(this->window);

    /* Handle rendering. */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw */
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* Update buffers and events. */
    glfwSwapBuffers(this->window);
    glfwPollEvents();
}


/**
 * Processes input generated by the user.
 */
void Video::processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}


/**
 * Checks if glfw received a closing message.
 */
bool Video::closeWindow()
{
    return glfwWindowShouldClose(this->window);
}
