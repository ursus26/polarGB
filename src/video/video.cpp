#include "video.h"
#include <iostream>
#include <stdlib.h>


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
        std::cout << "Failed to create GLFW window" << std::endl;
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
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    /**
     * Define a viewport for OpenGL.
     */
    glViewport(0, 0, this->width, this->height);
    glfwSetFramebufferSizeCallback(this->window, this->framebufferSizeCallback);
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