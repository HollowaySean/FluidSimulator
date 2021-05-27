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
    int resolution = 100;
    int totalSize = 500;
    int maxFrameRate = 100;
    float timeScale = 1.0;

    // Initialize state objects
    SimState testState = SimState(resolution);
    SimSource sources = SimSource(&testState);
    LoadState("default", &testState, &sources);

    // Set up simulation and control windows
    GLFWwindow* window = SimWindowSetup(resolution, totalSize);
    ControlWindowSetup(window);

    // Initialize timers
    SimTimer timer = SimTimer(maxFrameRate);
    timer.StartSimulation();

    // Simulation loop
    while(!glfwWindowShouldClose(window))
    {
        // Declare beginning of frame
        timer.StartFrame();

        // Draw current density to OpenGL window
        SimWindowRenderLoop(window, testState.fields.dens, testState.fields.temp);

        // Draw control window
        ControlWindowRenderLoop(window, &(testState.params));

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
    glfwDestroyWindow(window);
    glfwTerminate();

    // Exit code
    return 0;
}