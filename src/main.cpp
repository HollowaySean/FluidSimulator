// Pre-processor include statements
#include <iostream>
#include <cmath>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <nlohmann/json.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

using namespace ImGui;

// Project header files
#include "headers/SimSource.h"
#include "headers/SimState.h"
#include "headers/SimTimer.h"
#include "headers/Window.h"
#include "headers/StateLoader.h"

// Global variables
std::string projectPath;

int main(int argc, char** argv){

    // Get project path
    std::string fullpath = argv[0];
    projectPath = fullpath.substr(0, fullpath.find_last_of("/")) + "/..";

    // Window option
    int N = 100;
    int totalSize = 1000;
    int cellSize = int(round(totalSize / N));
    int maxFrameRate = 100;
    float timeScale = 1.0;

    // Initialize state objects
    SimState testState = SimState(N);
    SimSource sources = SimSource(&testState);
    LoadState("default", &testState, &sources);

    // Set up simulation window
    GLFWwindow* simWindow = SimWindowSetup(N, cellSize);

    // Set up control window
    // GLFWwindow* controlWindow = ControlWindowSetup(400, 400);

    // DEBUG SLASH TEST
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;   
    ImGui_ImplGlfw_InitForOpenGL(simWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Initialize timers
    SimTimer timer = SimTimer(maxFrameRate);
    timer.StartSimulation();

    float test = 0.0;

    // Simulation loop
    while(!glfwWindowShouldClose(simWindow))
    {
        // Declare beginning of frame
        timer.StartFrame();

        // Draw current density to OpenGL window
        SimWindowRenderLoop(simWindow, testState.fields.dens, testState.fields.temp);

        // Draw control window
        ControlWindowRenderLoop(simWindow, &test);

        // Update simulation state
        testState.SimulationStep(timeScale * timer.DeltaTime());

        // Sleep until frame is complete
        timer.EndFrame();

        // Read out frame rate every 100 frames
        timer.DisplayFrameRatePerMS(1000);
    }

    // End ImGui context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // End GLFW context
    glfwDestroyWindow(simWindow);
    // glfwDestroyWindow(controlWindow);
    glfwTerminate();

    // Exit code
    return 0;
}