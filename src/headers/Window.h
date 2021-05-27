/* Header file for window and OpenGL functions */

// Preprocessor statements
#ifndef WINDOW_H
#define WINDOW_H

// Include statements
#include "Shader.h"
#include "SimState.h"
#include "SimTimer.h"
#include "SimSource.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// Struct to carry window properties
struct WindowProps
{
    int resolution;
    int winWidth;
    int controlWidth;
    int maxFrameRate;
};

/// Setup methods ///

GLFWwindow* SimWindowSetup(int res, int windowWidth);
void ControlWindowSetup(GLFWwindow* window, int controlPanelWidth);

/// Main loop render methods ///

void SimWindowRenderLoop(GLFWwindow* window, float* density, float* temperature);
void ControlWindowRenderLoop(GLFWwindow* window, SimState* state, SimSource* source, SimTimer* timer);

/// Callbacks ///

void ErrorCallback(int error, const char* description);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);

/// Closing methods ///

void CloseWindows(GLFWwindow* window);


// Closing preprocessor statement
#endif