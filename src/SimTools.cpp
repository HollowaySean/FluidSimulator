/* Function definition file for simulation timers and counters */

// Include header definition
#include "SimTools.h"

// Includes and usings
#include <chrono>
#include <thread>
#include <iostream>
using namespace std;



//// PUBLIC METHODS ////

// Constructor
SimTimer::SimTimer(int maxFrameRate)
{
    this -> maxFrameRate = maxFrameRate;
    frameCounter = 0;
    totalFrameCounter = 0;
    frameTime = 1000.0 / float(maxFrameRate);
}

// Start simulation timer
void SimTimer::StartSimulation()
{
    simTimerStart = Now();
    monitorTimerStart = Now();
}

// Start frame timer, increment frame
void SimTimer::StartFrame()
{
    frameTimerStart = Now();
    frameCounter++;
    totalFrameCounter++;
}

// End frame and wait until frame time
void SimTimer::EndFrame()
{
    // Get current time
    unsigned int currentTime = Now();
    int elapsedTime = currentTime - frameTimerStart;
    int remainingTime = frameTime - elapsedTime;

    // Wait for remaining frame time
    if(remainingTime > 0){
        std::this_thread::sleep_for(std::chrono::milliseconds(remainingTime));
        timeStep = float(frameTime) / 1000.0;
    }else{
        timeStep = float(elapsedTime) / 1000.0;
    }
}

// Read out frame rate at set interval
void SimTimer::DisplayFrameRate(int framesPerUpdate)
{
    if(frameCounter % framesPerUpdate == 0){
        
        // Calculate frame rates
        unsigned int currentTime = Now();
        float currentFrameRate = (framesPerUpdate * 1000) / float((currentTime - monitorTimerStart));
        float avgFrameRate = (totalFrameCounter * 1000) / float((currentTime - simTimerStart));
        
        // Read out to std output
        cout << "Current FPS: " << currentFrameRate << ", Average FPS: " << avgFrameRate << endl;

        // Reset frame counter
        frameCounter = 0;

        // Reset monitor timer
        monitorTimerStart = Now();
    }
}

// Get frame time for simulation
float SimTimer::DeltaTime()
{
    return timeStep;
}

// Get current run time in milliseconds
int SimTimer::RunTime()
{
    return Now() - simTimerStart;
}



//// PRIVATE METHODS ////

// Get current time in milliseconds
unsigned int SimTimer::Now()
{
    return chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch()).count();
}