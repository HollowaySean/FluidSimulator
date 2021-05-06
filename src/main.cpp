// Pre-processor include statements
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "SimState.h"

// Namespaces
using namespace std;

// Macros
#define ind(i,j,N) ((i) + ((N) + 2)*(j))

// Function definition before main
void DisplayGLWindow(SimState, int);

int main(int argc, char** argv){

    // Window option
    int N = 60;
    int totalSize = 1000;
    int cellSize = int(round(totalSize / N));
    int size = (N + 2) * (N + 2);

    // Declarations
    float lengthScale = 0.1;
    float visc = 0.00001 / (lengthScale * lengthScale);
    float diff = 0.00001 / (lengthScale * lengthScale);
    float maxFrameRate = 25.0;
    float dt = 1 / maxFrameRate;
    unsigned int frameTime = uint(dt * 1000);

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
    
    // Start timer
    chrono::milliseconds msNow = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch());
    unsigned int simStartTime = msNow.count();
    unsigned int startTime = simStartTime;
    unsigned int frameTimer = simStartTime;

    // Set up source strength & direction
    float strength = 50.0;
    float angle = -91.0;

    int frameCount = 1;
    int lagFrames = 0;

    // Simulation loop
    while(true)
    {
        // Draw current density to OpenGL window
        DisplayGLWindow(testState, cellSize);


        // Set sources
        unsigned int msNowInt = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch()).count();

        // Randomize
        angle += static_cast <float> ((rand() % 3) - 1) * 2.0;
        strength += static_cast <float> ((rand() % 3) - 1) * 0.5;

        // Add sources for first 10 seconds
        if(msNowInt - simStartTime < 10000){
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
        testState.SimulationStep(dt);

        // Sleep until frame time is complete
        unsigned int currentTime = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch()).count();
        int timeElapsed = currentTime - startTime;
        if(timeElapsed < frameTime)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(frameTime - timeElapsed));
        }else{
            lagFrames++;
        }

        // Read out frame rate every 100 frames
        if(frameCount == 100){

            // Calculate frame rate
            float frameRate = round((1000 * 100) / (currentTime - frameTimer));
            frameTimer = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch()).count();            

            // Read out stats
            cout << "Current frame rate: " << frameRate << endl;
            // cout << "Frames below max FPS: " << lagFrames << endl;

            // Reset counters
            frameCount = 0;
            lagFrames = 0;
        }
        frameCount++;

        // Reset frame timer
        startTime = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch()).count();
    }

    // Exit code
    return 0;
}

void DisplayGLWindow(SimState currentState, int cellSize)
{
    int pixelsPerSquare = cellSize;
    int N = currentState.GetN();
    float brightnessFactor = 1.0;

    // Get density map
    float * brightness = currentState.GetDensity();
    for(int i = 0; i < currentState.GetSize(); i++){

        brightness[i] = (brightness[i] > (1.0 / brightnessFactor)) ? 1.0 : brightnessFactor * brightness[i];
    }

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