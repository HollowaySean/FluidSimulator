/* Header file for simulation timer */

#ifndef SIMTIMER_H
#define SIMTIMER_H

class SimTimer
{
    public:

        // Constructor
        SimTimer(int maxFrameRate);

        // Public methods
        void StartSimulation();
        void StartFrame();
        void EndFrame();
        void DisplayFrameRate(int framesPerUpdate);
        void DisplayFrameRatePerMS(int msPerUpdate);
        void TrackFrameRatePerMS(int msPerUpdate);

        // Public accessors
        void SetFrameRate(int maxFrameRate);
        float DeltaTime();
        int RunTime();
        int CurrentFrame();
        int MaxFrameRate();
        float CurrentFrameRate();
        float AverageFrameRate();

    private:

        // Private methods
        unsigned int Now();
        void UpdateFrameRate();

        // Variables and counters
        int frameCounter;
        int totalFrameCounter;
        int maxFrameRate;
        int frameTime;
        float timeStep;
        int msPerUpdate;
        float currentFrameRate = 0.0;
        float averageFrameRate = 0.0;
        bool trackFrameRate = false;

        // Timers
        unsigned int simTimerStart;
        unsigned int frameTimerStart;
        unsigned int monitorTimerStart;
};

#endif