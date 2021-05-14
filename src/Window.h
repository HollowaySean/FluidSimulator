/* Header file for window and OpenGL functions */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void WindowTest(GLFWwindow* window);

void ErrorCallback(int error, const char* description);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);