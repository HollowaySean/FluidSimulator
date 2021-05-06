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
    int totalSize = 500;
    int cellSize = int(round(totalSize / N));
    int size = (N + 2) * (N + 2);
    int maxFrameRate = 60;

    // Declarations
    float lengthScale = 0.1;
    float visc = 0.00001 / (lengthScale * lengthScale);
    float diff = 0.00001 / (lengthScale * lengthScale);

    // Array initializations
    float dens_source[size] = { 0 };
    float u_source[size] = { 0 };
    float v_source[size] = { 0 };

    // Set up source
    int sourceLocation = ind(int(ceil(N/2)), int(ceil(N/2)), N);
    u_source[sourceLocation] = 100.0;
    v_source[sourceLocation] = 0.0;

    // Initialize state
    SimState testState = SimState(N, visc, diff);
    testState.SetSources(dens_source, u_source, v_source);

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

    // Set up source strength & direction
    float strength = 50.0;
    float angle = -91.0;

    // Simulation loop
    while(true)
    {
        // Declare beginning of frame
        timer.StartFrame();

        // Draw current density to OpenGL window
        DisplayGLWindow(testState, cellSize);

        // Randomize
        angle += static_cast <float> ((rand() % 3) - 1) * 2.0;
        strength += static_cast <float> ((rand() % 3) - 1) * 0.5;

        // Add sources for first 10 seconds
        if(timer.RunTime() < 10000){
            dens_source[sourceLocation] = 100.0;
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
    }

    // Exit code
    return 0;
}

void DisplayGLWindow(SimState currentState, int cellSize)
{
    // Set properties
    int pixelsPerSquare = cellSize;
    int N = currentState.GetN();

    // Get density map
    float * brightness = currentState.GetDensity();

    // Clear OpenGL window
    glClear(GL_COLOR_BUFFER_BIT);

    // Loop through each grid cell and draw a polygon
    for(int x = 0; x <= N - 1; x++){
        for(int y = 0; y <= N - 1; y++){


            glBegin(GL_QUADS);
            glColor3f(brightness[ind(x,y,N)], brightness[ind(x,y,N)], brightness[ind(x,y,N)]);
            glVertex2i(pixelsPerSquare * x,     pixelsPerSquare * y);
            glColor3f(brightness[ind(x+1,y,N)], brightness[ind(x+1,y,N)], brightness[ind(x+1,y,N)]);
            glVertex2i(pixelsPerSquare * (x+1), pixelsPerSquare * y);
            glColor3f(brightness[ind(x+1,y+1,N)], brightness[ind(x+1,y+1,N)], brightness[ind(x+1,y+1,N)]);
            glVertex2i(pixelsPerSquare * (x+1), pixelsPerSquare * (y+1));
            glColor3f(brightness[ind(x,y+1,N)], brightness[ind(x,y+1,N)], brightness[ind(x,y+1,N)]);
            glVertex2i(pixelsPerSquare * x,     pixelsPerSquare * (y+1));
            glEnd();
            glFlush();
        }
    }

    // Complete OpenGL loop
    glutSwapBuffers();

}