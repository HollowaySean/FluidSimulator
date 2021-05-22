// Pre-processor include statements
#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <nlohmann/json.hpp>

// Project header files
#include "headers/SimSource.h"
#include "headers/SimState.h"
#include "headers/SimTimer.h"
#include "headers/Window.h"
#include "headers/StateLoader.h"

// Namespaces
using namespace std;

// Macros
#define ind(i,j,N) ((i) + ((N) + 2)*(j))

int main(int argc, char** argv){

    // Window option
    int N = 100;
    int totalSize = 1000;
    int cellSize = int(round(totalSize / N));
    int maxFrameRate = 100;
    float timeScale = 1.0;

    // Initialize state objects
    SimState testState = SimState(N);
    SimSource sources = SimSource(&testState);
    LoadState("./src/json/default.json", &testState, &sources);

    // Set up sources
    sources.CreateGasSource(SimSource::circle, 0.5, 5000.0, 0.0, -0.75, 0.075);
    sources.CreateWindBoundary(0.025);
    sources.UpdateSources();

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
        timer.DisplayFrameRate(100);

        // Exit after 1000 frames
        if(timer.CurrentFrame() == 1000){ break; }
    }

    // End GLFW context
    glfwTerminate();

    // Exit code
    return 0;
}