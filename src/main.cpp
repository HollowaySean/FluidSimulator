// Pre-processor include statements
#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Project header files
#include "SimTools.h"

// Namespaces
using namespace std;

// Macros
#define ind(i,j,N) ((i) + ((N) + 2)*(j))

// Function definition before main
void DisplayGLWindow(SimState, int);

int main(int argc, char** argv){

    // Window option
    int N = 75;
    int totalSize = 1000;
    int cellSize = int(round(totalSize / N));
    int maxFrameRate = 25;

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
    testState.SetBoundaryClosed(false);
    SimSource sources = SimSource(&testState);

    // Set up sources
    sources.CreateGasSource(SimSource::circle, 0.5, 2000.0, 0.0, -0.75, 0.075);
    sources.CreateWindBoundary(0.025);
    sources.UpdateSources();



    // Set up OpenGL state
    glfwInit();
    // glutInit(&argc, argv);
    // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    // glutInitWindowSize(cellSize * N, cellSize * N);
    // glutInitWindowPosition(100, 100);
    // wd = glutCreateWindow("Fluid Simulator");

    // glutKeyboardFunc(kbd);

    glClearColor(0.0,0.0,0.0,0.0);
    glOrtho(0, cellSize * N, 0, cellSize * N, -1., 1.);

    // Set up GLEW environment
    GLenum err = glewInit();
    if(GLEW_OK != err){
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    


    // Initialize timers
    SimTimer timer = SimTimer(maxFrameRate);
    timer.StartSimulation();

    // Simulation loop
    while(true)
    {
        // Declare beginning of frame
        timer.StartFrame();

        // Draw current density to OpenGL window
        DisplayGLWindow(testState, cellSize);

        // Update simulation state
        testState.SimulationStep(timeScale * timer.DeltaTime());

        // Sleep until frame is complete
        timer.EndFrame();

        // Read out frame rate every 100 frames
        timer.DisplayFrameRate(100);

        // Exit after 1000 frames
        if(timer.CurrentFrame() == 10000){ break; }
    }

    // Exit code
    return 0;
}

float LerpColor(float densIn, float tempIn, int channel)
{
    // Parameters
    float bMod = 0.0000000000001;
    // float bMod = 1.0;

    float t = (tempIn - 1900.0) / (3200. - 1900.);
    float output = 0.0;

    float intensity = bMod * densIn * tempIn * tempIn * tempIn * tempIn;
    // float intensity = bMod * densIn;

    if(intensity > 1.0){ intensity = 1.0; }
    float color;

    switch(channel){
        case 1:
            color = 1.0;
            break;
        case 2:
            color = t * (1.0 - 0.5765) + 0.5765;
            break;
        case 3:
            color = t * (1.0 - 0.1608) + 0.1608;
            break;
    }

    output = intensity * color;
    // output = intensity;

    return output;
}

void DisplayGLWindow(SimState currentState, int cellSize)
{
    // Set properties
    int pixelsPerSquare = cellSize;
    int N = currentState.GetN();

    // Get density map
    float * density = currentState.GetDensity();
    float * temperature = currentState.GetTemperature();

    // Clear OpenGL window
    glClear(GL_COLOR_BUFFER_BIT);

    // Loop through each grid cell and draw a polygon
    for(int x = 1; x <= N; x++){
        for(int y = 1; y <= N; y++){

            glBegin(GL_QUADS);

            glColor3f(  LerpColor(density[ind(x,y+1,N)], temperature[ind(x,y+1,N)], 1), 
                        LerpColor(density[ind(x,y+1,N)], temperature[ind(x,y+1,N)], 2), 
                        LerpColor(density[ind(x,y+1,N)], temperature[ind(x,y+1,N)], 3));
            glVertex2i( pixelsPerSquare * (x),     
                        pixelsPerSquare * (y+1));

            glColor3f(  LerpColor(density[ind(x+1,y+1,N)], temperature[ind(x+1,y+1,N)], 1), 
                        LerpColor(density[ind(x+1,y+1,N)], temperature[ind(x+1,y+1,N)], 2), 
                        LerpColor(density[ind(x+1,y+1,N)], temperature[ind(x+1,y+1,N)], 3));
            glVertex2i( pixelsPerSquare * (x+1), 
                        pixelsPerSquare * (y+1));

            glColor3f(  LerpColor(density[ind(x+1,y,N)], temperature[ind(x+1,y,N)],  1), 
                        LerpColor(density[ind(x+1,y,N)], temperature[ind(x+1,y,N)],  2), 
                        LerpColor(density[ind(x+1,y,N)], temperature[ind(x+1,y,N)],  3));
            glVertex2i( pixelsPerSquare * (x+1), 
                        pixelsPerSquare * (y));

            glColor3f(  LerpColor(density[ind(x,y,N)], temperature[ind(x,y,N)],  1), 
                        LerpColor(density[ind(x,y,N)], temperature[ind(x,y,N)],  2), 
                        LerpColor(density[ind(x,y,N)], temperature[ind(x,y,N)],  3));
            glVertex2i( pixelsPerSquare * (x),     
                        pixelsPerSquare * (y));

            glEnd();
            glFlush();
        }
    }

    // Complete OpenGL loop
    // glutSwapBuffers();
}