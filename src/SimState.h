/* Header file for simulation state class */


class SimState
{
    public:

        // Constructors
        SimState(int N);
        SimState(int N, float viscosity, float diffusion, float gravity, float airDensity);
        SimState(int N, SimParams params);

        // Public methods
        void SetSources(float * density, float * xVelocity, float * yVelocity);
        void SimulationStep(float timeStep);

        // Array accessors
        float * GetDensity();
        float * GetXVelocity();
        float * GetYVelocity();

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
        void SetSource(float *, float *);
        void AddSource(float *, float *, float);
        void AddConstantSource(float *, float, float);

        void Gravitate(float *, float *, float, float, float);
        void Diffuse(int, float *, float *, float, float);
        void Advect(int, float *, float *, float *, float *, float);

        void SetBoundary(int, float *);
        void HodgeProjection(float *, float *, float *, float *);

        void DensityStep(float);
        void VelocityStep(float);

        // Array struct
        SimFields fields;
};

// Structure to hold onto simulation properties and physical constants
struct SimParams
{
    // Constructors
    SimParams();
    SimParams(float viscosity, float diffusion, float gravity, float airDensity);

    // Options
    bool gravityOn;
    int solverSteps;

    // Physical constants
    float visc;
    float diff;
    float grav;
    float airDens;
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

    // Previous grid
    float * xVel_prev;
    float * yVel_prev;
    float * dens_prev;

    // Source grid
    float * xVel_source;
    float * yVel_source;
    float * dens_source;
};

// Method to display simulation state in terminal
void DisplayGrid(SimState stateInput, float minimum);