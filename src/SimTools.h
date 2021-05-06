/* Header file for simulation tools */

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

        // Public accessors
        void SetFrameRate(int maxFrameRate);
        float DeltaTime();
        int RunTime();
        int CurrentFrame();

    private:

        // Private methods
        unsigned int Now();

        // Variables and counters
        int frameCounter;
        int totalFrameCounter;
        int maxFrameRate;
        int frameTime;
        float timeStep;

        // Timers
        unsigned int simTimerStart;
        unsigned int frameTimerStart;
        unsigned int monitorTimerStart;
};