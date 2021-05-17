/* Header file for window and OpenGL functions */

// Preprocessor statements
#ifndef WINDOW_H
#define WINDOW_H

#include "Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

GLFWwindow* WindowSetup(int xSize, int ySize);
void WindowRenderLoop(GLFWwindow* window, float* density, float* temperature);

void ErrorCallback(int error, const char* description);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);

// Closing preprocessor statement
#endif