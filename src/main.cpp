// Pre-processor include statements
#include <iostream>
#include <cmath>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <nlohmann/json.hpp>

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

    // Set up window
    GLFWwindow* window = WindowSetup(N, cellSize);

    // Initialize timers
    SimTimer timer = SimTimer(maxFrameRate);
    timer.StartSimulation();

    // Simulation loop
    while(!glfwWindowShouldClose(window))
    {
        // Declare beginning of frame
        timer.StartFrame();

        // Draw current density to OpenGL window
        WindowRenderLoop(window, testState.fields.dens, testState.fields.temp);

        // Update simulation state
        testState.SimulationStep(timeScale * timer.DeltaTime());

        // Sleep until frame is complete
        timer.EndFrame();

        // Read out frame rate every 100 frames
        timer.DisplayFrameRatePerMS(1000);
    }

    // End GLFW context
    glfwTerminate();

    // Exit code
    return 0;
}