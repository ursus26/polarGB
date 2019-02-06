#ifndef VIDEO_H
#define VIDEO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Video
{
public:
    Video();
    ~Video();

    void initialise();
    void cleanUp();
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void update();
    bool closeWindow();
    void processInput(GLFWwindow* w);

private:
    GLFWwindow* window;
    std::string windowName;
    int width;
    int height;
};


#endif /* VIDEO_H */
