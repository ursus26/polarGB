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

#ifndef VIDEO_H
#define VIDEO_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>

class Video
{
public:
    Video();
    ~Video();

    void initialise();
    void cleanUp();

    void update();

    void drawFrame();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    bool closeWindow();
    void processInput(GLFWwindow* w);

private:
    GLFWwindow* window;
    std::string windowName;
    int width;
    int height;

    /* Shaders */
    unsigned int shaderProgram;


    void initShaders();
};


#endif /* VIDEO_H */
