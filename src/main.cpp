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
    int maxFrameRate = 25;

    // Declarations
    float lengthScale = 1.0;
    float visc = 0.00001;
    float diff = 0.000000001;
    float grav = -9.8;
    float airDensity = 1.29235;
    // float massRatio = 0.5416;
    float massRatio = 0.9;

    // Array initializations
    float dens_source[size] = { 0 };
    float u_source[size] = { 0 };
    float v_source[size] = { 0 };



    // Initialize state
    SimParams params = SimParams(lengthScale, visc, diff, grav, airDensity, massRatio);
    SimState testState = SimState(N, params);
    testState.SetSources(dens_source, u_source, v_source);
    testState.params.gravityOn = true;

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
    


    // Initialize timers
    SimTimer timer = SimTimer(maxFrameRate);
    timer.StartSimulation();

    // Set up source
    float strength = 100.0;
    float angle = 90.0;
    float thickness = 10.0;
    int sourceLocation = ind(int(ceil(N/2)), 5, N);



    // Simulation loop
    while(true)
    {
        // Declare beginning of frame
        timer.StartFrame();

        // Draw current density to OpenGL window
        DisplayGLWindow(testState, cellSize);

        // Randomize
        // angle += (static_cast <float> ((rand() % 11) - 5) * 10.0) * timer.DeltaTime();
        // strength += (static_cast <float> ((rand() % 3) - 1) * 0.0) * timer.DeltaTime();
        // thickness += (static_cast <float> ((rand() % 3) - 1) * 10.0) * timer.DeltaTime();

        // Add sources for first 10 seconds
        if(timer.RunTime() < 20000){
            dens_source[sourceLocation] = thickness;
            u_source[sourceLocation] = strength * cos(angle * 3.141592/180.0);
            v_source[sourceLocation] = strength * sin(angle * 3.141592/180.0);
        }else{
            dens_source[sourceLocation] = 0.0;
            u_source[sourceLocation] = 0.0;
            v_source[sourceLocation] = 0.0;
        }

        // Update sources
        testState.SetSources(dens_source, u_source, v_source);

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

void DisplayGLWindow(SimState currentState, int cellSize)
{
    // Set properties
    int pixelsPerSquare = cellSize;
    int N = currentState.GetN();
    float bMod = 1.0;

    // Get density map
    float * brightness = currentState.GetDensity();

    // Clear OpenGL window
    glClear(GL_COLOR_BUFFER_BIT);

    // Loop through each grid cell and draw a polygon
    for(int x = 0; x <= N - 1; x++){
        for(int y = 0; y <= N - 1; y++){

            glBegin(GL_QUADS);

            glColor3f(  bMod * brightness[ind(x,y,N)], 
                        bMod * brightness[ind(x,y,N)], 
                        bMod * brightness[ind(x,y,N)]);
            glVertex2i( pixelsPerSquare * x,     
                        pixelsPerSquare * y);

            glColor3f(  bMod * brightness[ind(x+1,y,N)], 
                        bMod * brightness[ind(x+1,y,N)], 
                        bMod * brightness[ind(x+1,y,N)]);
            glVertex2i( pixelsPerSquare * (x+1), 
                        pixelsPerSquare * y);

            glColor3f(  bMod * brightness[ind(x+1,y+1,N)], 
                        bMod * brightness[ind(x+1,y+1,N)], 
                        bMod * brightness[ind(x+1,y+1,N)]);
            glVertex2i( pixelsPerSquare * (x+1), 
                        pixelsPerSquare * (y+1));

            glColor3f(  bMod * brightness[ind(x,y+1,N)], 
                        bMod * brightness[ind(x,y+1,N)], 
                        bMod * brightness[ind(x,y+1,N)]);
            glVertex2i( pixelsPerSquare * x,     
                        pixelsPerSquare * (y+1));

            glEnd();
            glFlush();
        }
    }

    // Complete OpenGL loop
    glutSwapBuffers();
}