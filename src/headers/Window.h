/* Header file for window and OpenGL functions */

// Preprocessor statements
#ifndef WINDOW_H
#define WINDOW_H

#include "Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


GLFWwindow* SimWindowSetup(int xSize, int ySize);
GLFWwindow* ControlWindowSetup(int xSize, int ySize);
void SimWindowRenderLoop(GLFWwindow* window, float* density, float* temperature);
void ControlWindowRenderLoop(GLFWwindow* window, float* controlVal);

void ErrorCallback(int error, const char* description);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);

// Closing preprocessor statement
#endif