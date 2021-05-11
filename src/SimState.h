/* Header file for simulation state class */



// Structure to hold onto simulation properties and physical constants
struct SimParams
{
    // Constructors
    SimParams();
    SimParams(float lengthScale, float viscosity, float diffusion);
    SimParams(float lengthScale, float viscosity, float diffusion, 
                float gravity, float airDensity, float massRatio);
    SimParams(float lengthScale, float viscosity, float diffusion, 
                float gravity, float airDensity, float massRatio, 
                float airTemp, float diffTemp, float expansionTemp);

    // Options
    bool gravityOn;
    bool temperatureOn;
    int solverSteps;

    // Physical constants
    float lengthScale;
    float visc;
    float diff;
    float grav;
    float airDens;
    float massRatio;
    float airTemp;
    float diffTemp;
    float expansionTemp;
};

// Structure to hold onto array pointers
struct SimFields
{
    // Constructors
    SimFields();
    SimFields(int size);

    // Current grid
    float * xVel;
    float * yVel;
    float * dens;
    float * temp;

    // Previous grid
    float * xVel_prev;
    float * yVel_prev;
    float * dens_prev;
    float * temp_prev;

    // Source grid
    float * xVel_source;
    float * yVel_source;
    float * dens_source;
    float * temp_source;
};


// Class which defines and contains important simulation methods
class SimState
{
    public:

        // Constructors
        SimState(int N);
        SimState(int N, SimParams params);

        // Public methods
        void SetSources(float * density, float * xVelocity, float * yVelocity, float * temperature);
        void SimulationStep(float timeStep);

        // Array accessors
        float * GetDensity();
        float * GetXVelocity();
        float * GetYVelocity();
        float * GetTemperature();

        // Grid size accessors
        int GetN();
        int GetSize();

        // Parameter struct
        SimParams params;

    private:

        // Grid size
        int N;
        int size;

        // Internal Methods
        void ZeroArrays();
        void SetSource(float *, float *);
        void SetConstantSource(float *, float);
        void AddSource(float *, float *, float);
        void AddConstantSource(float *, float, float);

        void Diffuse(int, float *, float *, float, float);
        void Advect(int, float *, float *, float *, float *, float);
        void Gravitate(float *, float *, float, float, float, float);
        void ThermalDecay(float *, float, float, float);

        void SetBoundary(int, float *);
        void HodgeProjection(float *, float *, float *, float *);

        void DensityStep(float);
        void VelocityStep(float);
        void TemperatureStep(float);

        // Array struct
        SimFields fields;
};

// Method to display simulation state in terminal
void DisplayGrid(SimState stateInput, float minimum);