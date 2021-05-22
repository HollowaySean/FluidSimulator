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

    // Declarations
    float lengthScale = 1.0;        // Side length of the box
    float timeScale = 1.0;          // Simulation speed in simulation seconds / real seconds
    float visc = 0.000018;          // Dynamic viscosity at air temperature
    float diff = 0.000028;          // Diffusion constant at air temperature
    float grav = -9.8;              // Gravitational force
    float airDensity = 1.29235;     // Density of background air
    // float massRatio = 1.608;        // Ratio of molar mass of air / molar mass of gas
    float massRatio = 0.54;         // Ratio of molar mass of air / molar mass of gas
    float airTemp = 300.0;          // Temperature of background air
    float diffTemp = 0.00002338;    // Thermal diffusivity of gas
    float densDecay = 0.0;          // Rate of decay for density field
    float tempFactor = 0.0;         // Linear decrease in density decay per degree temperature above airTemp
    float tempDecay = 0.0;          // Rate of decay for temperature field



    // Initialize state
    SimParams params = SimParams(lengthScale, visc, diff, grav, airDensity, massRatio, airTemp, diffTemp, densDecay, tempFactor, tempDecay);
    SimState testState = SimState(N, params);
    SimSource sources = SimSource(&testState);

    // Set up sources
    testState.SetBoundaryClosed(false);
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
        if(timer.CurrentFrame() == 10000){ break; }
    }

    // End GLFW context
    glfwTerminate();

    // Exit code
    return 0;
}