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

    // Initialize state objects
    WindowProps props;
    LoadWindow("default", &props);
    SimState testState = SimState(props.resolution);
    SimSource sources = SimSource(&testState);
    LoadState("default", &testState, &sources);

    // Set up simulation and control windows
    GLFWwindow* window = SimWindowSetup(props.resolution, props.winWidth);
    ControlWindowSetup(window, props.controlWidth);

    // Initialize timers
    SimTimer timer = SimTimer(props.maxFrameRate);
    timer.TrackFrameRatePerMS(1000);
    timer.StartSimulation();

    // Simulation loop
    while(!glfwWindowShouldClose(window))
    {
        // Declare beginning of frame
        timer.StartFrame();

        // Draw current density to OpenGL window
        SimWindowRenderLoop(window, testState.fields.dens, testState.fields.temp);

        // Draw control window
        ControlWindowRenderLoop(window, &(testState.params), &timer);

        // Update simulation state
        testState.SimulationStep(timer.DeltaTime());

        // Sleep until frame is complete
        timer.EndFrame();
    }

    CloseWindows(window);

    // Exit code
    return 0;
}