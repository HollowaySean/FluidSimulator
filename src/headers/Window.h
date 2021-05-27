/* Header file for window and OpenGL functions */

// Preprocessor statements
#ifndef WINDOW_H
#define WINDOW_H

#include "Shader.h"
#include "SimState.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


GLFWwindow* SimWindowSetup(int res, int windowWidth);
void ControlWindowSetup(GLFWwindow* window);

void SimWindowRenderLoop(GLFWwindow* window, float* density, float* temperature);
void ControlWindowRenderLoop(GLFWwindow* window, SimParams* params);

void ErrorCallback(int error, const char* description);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);

// Closing preprocessor statement
#endif