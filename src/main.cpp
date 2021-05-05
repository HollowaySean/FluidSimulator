// Pre-processor include statements
#include <chrono>
#include <thread>
#include <iostream>
#include <GL/glut.h>
#include "SimState.h"

// Namespaces
using namespace std;

// Macros
#define ind(i,j,N) ((i) + ((N) + 2)*(j))

// Function definition before main
void DisplayGLWindow(SimState);

int main(int argc, char** argv){

    // Declarations
    int N = 25;
    int size = (N + 2) * (N + 2);
    float visc = 10;
    float diff = 10;
    float dt = 1;

    // Array initializations
    float dens_source[size] = { 0 };
    float u_source[size] = { 0 };
    float v_source[size] = { 0 };

    // Set conditions
    for(int i = 0; i <= N+1; i++){
        for(int j = 0; j <= N+1; j++){
            u_source[ind(i,j,N)] = 100.0;
            v_source[ind(i,j,N)] = 100.0;
        }
    }
    dens_source[ind(13,13,N)] = 100.0;

    // Initialize state
    SimState testState = SimState(N, visc, diff);
    testState.SetSources(dens_source, u_source, v_source);

    // Set up OpenGL state
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(10 * N, 10 * N);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fluid Simulator");

    glClearColor(0.0,0.0,0.0,0.0);
    gluOrtho2D(0, 10 * N, 0, 10 * N);

    // Simulation loop
    while(true)
    {
        // Draw current density to OpenGL window
        DisplayGLWindow(testState);

        // Update simulation state
        testState.SimulationStep(dt);

        // Sleep for one second (improve this)
        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000 * dt)));

    }

    // Exit code
    return 0;
}

void DisplayGLWindow(SimState currentState)
{
    int pixelsPerSquare = 10;
    int N = currentState.GetN();

    // Get density map
    float * brightness = currentState.GetDensity();
    for(int i = 0; i < currentState.GetSize(); i++){

        brightness[i] = (brightness[i] > (1.0 / 10)) ? 1.0 : 1000 * brightness[i];
    }

    // Clear OpenGL window
    glClear(GL_COLOR_BUFFER_BIT);

    // Loop through each grid cell and draw a polygon
    for(int x = 0; x <= N - 1; x++){
        for(int y = 0; y <= N - 1; y++){


            glBegin(GL_QUADS);
            // glColor3f(brightness[ind(x,y,N)], brightness[ind(x,y,N)], brightness[ind(x,y,N)]);
            glColor3f(x / 25, y / 25, 1);
            glVertex2i(pixelsPerSquare * x,     pixelsPerSquare * y);
            glVertex2i(pixelsPerSquare * (x+1), pixelsPerSquare * y);
            glVertex2i(pixelsPerSquare * (x+1), pixelsPerSquare * (y+1));
            glVertex2i(pixelsPerSquare * x,     pixelsPerSquare * (y+1));
            glEnd();
            glFlush();
        }
    }

    // Complete OpenGL loop
    glutSwapBuffers();

}