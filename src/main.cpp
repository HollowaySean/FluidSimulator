// Pre-processor include statements
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SimState.h"
#include "SimTools.h"

// Namespaces
using namespace std;

// Macros
#define ind(i,j,N) ((i) + ((N) + 2)*(j))

// Function definition before main
void DisplayGLWindow(SimState, int);

int main(int argc, char** argv){

    // Window option
    int N = 50;
    int totalSize = 1000;
    int cellSize = int(round(totalSize / N));
    int size = (N + 2) * (N + 2);
    int maxFrameRate = 60;

    // Declarations
    float lengthScale = 1.0;        // Side length of the box
    float visc = 0.000018;          // Dynamic viscosity at air temperature
    float diff = 0.000028;          // Diffusion constant at air temperature
    float grav = -9.8;              // Gravitational force
    float airDensity = 1.29235;     // Density of background air
    // float massRatio = 1.608;       // Ratio of molar mass of air / molar mass of gas
    float massRatio = 0.54;       // Ratio of molar mass of air / molar mass of gas
    float airTemp = 300.0;          // Temperature of background air
    float diffTemp = 0.00002338;    // Thermal diffusivity of gas
    float densDecay = 100.0;        // Rate of decay for density field
    float tempDecay = 0.0;            // Rate of decay for temperature field



    // Initialize state
    SimParams params = SimParams(lengthScale, visc, diff, grav, airDensity, massRatio, airTemp, diffTemp, densDecay, tempDecay);
    SimState testState = SimState(N, params);
    SimSource sources = SimSource(&testState);

    // Set up OpenGL state
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(cellSize * N, cellSize * N);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fluid Simulator");
    glClearColor(0.0,0.0,0.0,0.0);
    gluOrtho2D(0, cellSize * N, 0, cellSize * N);

    // Set up GLEW environment
    GLenum err = glewInit();
    if(GLEW_OK != err){
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    

    // Set up sources
    sources.CreateGasSource(SimSource::square, 0.1, 0.0, 0.0, 0.0, 0.1);
    sources.UpdateSources();

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
        testState.SimulationStep(timer.DeltaTime());

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
    // float bMod = 1000.0;
    float bMod = 0.000000000001;

    float t = (tempIn - 0.0) / (1300. - 0.);
    float output = 0.0;

    // float intensity = bMod * densIn * tempIn * tempIn * tempIn * tempIn;
    float intensity = 1.0;
    if(intensity > 1.0){ intensity = 1.0; }
    float color;

    switch(channel){
        case 1:
            // color = 1.0;
            color = t;
            break;
        case 2:
            // color = t * (1.0 - 0.5765) + 0.5765;
            color = 0.0;
            break;
        case 3:
            // color = t * (1.0 - 0.1608) + 0.1608;
            color = 1.0 - t;
            break;
    }

    output = intensity * color;

    return output;
}

void DisplayGLWindow(SimState currentState, int cellSize)
{
    // Set properties
    int pixelsPerSquare = cellSize;
    int N = currentState.GetN();

    // Get density map
    float * density = currentState.GetDensity();
    // float * temperature = currentState.GetTemperature();
    float * temperature = currentState.fields.temp_source;

    // Clear OpenGL window
    glClear(GL_COLOR_BUFFER_BIT);

    // Loop through each grid cell and draw a polygon
    for(int x = 0; x <= N - 1; x++){
        for(int y = 0; y <= N - 1; y++){

            glBegin(GL_QUADS);

            glColor3f(  LerpColor(density[ind(x,y,N)], temperature[ind(x,y,N)],  1), 
                        LerpColor(density[ind(x,y,N)], temperature[ind(x,y,N)],  2), 
                        LerpColor(density[ind(x,y,N)], temperature[ind(x,y,N)],  3));
            glVertex2i( pixelsPerSquare * x,     
                        pixelsPerSquare * y);

            glColor3f(  LerpColor(density[ind(x+1,y,N)], temperature[ind(x+1,y,N)],  1), 
                        LerpColor(density[ind(x+1,y,N)], temperature[ind(x+1,y,N)],  2), 
                        LerpColor(density[ind(x+1,y,N)], temperature[ind(x+1,y,N)],  3));
            glVertex2i( pixelsPerSquare * (x+1), 
                        pixelsPerSquare * y);

            glColor3f(  LerpColor(density[ind(x+1,y+1,N)], temperature[ind(x+1,y+1,N)], 1), 
                        LerpColor(density[ind(x+1,y+1,N)], temperature[ind(x+1,y+1,N)], 2), 
                        LerpColor(density[ind(x+1,y+1,N)], temperature[ind(x+1,y+1,N)], 3));
            glVertex2i( pixelsPerSquare * (x+1), 
                        pixelsPerSquare * (y+1));

            glColor3f(  LerpColor(density[ind(x,y+1,N)], temperature[ind(x,y+1,N)], 1), 
                        LerpColor(density[ind(x,y+1,N)], temperature[ind(x,y+1,N)], 2), 
                        LerpColor(density[ind(x,y+1,N)], temperature[ind(x,y+1,N)], 3));
            glVertex2i( pixelsPerSquare * x,     
                        pixelsPerSquare * (y+1));

            glEnd();
            glFlush();
        }
    }

    // Complete OpenGL loop
    glutSwapBuffers();
}