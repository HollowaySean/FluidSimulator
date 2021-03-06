/* Header file for simulation state class */

// Preprocessor statements
#ifndef SIMSTATE_H
#define SIMSTATE_H

#include <string>

// Structure to hold onto simulation properties and physical constants
struct SimParams
{
    // Constructors
    SimParams();
    SimParams(float lengthScale, float timeScale, float viscosity, float diffusion);
    SimParams(float lengthScale, float timeScale, float viscosity, float diffusion, 
                float gravity, float airDensity, float massRatio);
    SimParams(float lengthScale, float timeScale, float viscosity, float diffusion, 
                float gravity, float airDensity, float massRatio, 
                float airTemp, float diffTemp);
    SimParams(float lengthScale, float timeScale, float viscosity, float diffusion, 
                float gravity, float airDensity, float massRatio, 
                float airTemp, float diffTemp,
                float densDecay, float tempFactor, float tempDecay);

    // Reference tools for live modification
    enum ParamType { scale, fluid, background, decay };
    float*      FloatPointer (int paramNum, ParamType type);
    float       FloatMin     (int paramNum, ParamType type);
    float       FloatMax     (int paramNum, ParamType type);
    std::string FloatName    (int paramNum, ParamType type);
    std::string FloatTip     (int paramNum, ParamType type);

    // Options
    bool closedBoundaries;
    bool advancedCoefficients;
    bool gravityOn;
    bool temperatureOn;
    int solverSteps;

    // Physical constants
    float lengthScale;
    float timeScale;
    float visc;
    float diff;
    float grav;
    float airDens;
    float massRatio;
    float airTemp;
    float diffTemp;
    float densDecay;
    float tempFactor;
    float tempDecay;
};

// Structure to hold onto array pointers
struct SimFields
{
    // Constructors
    SimFields();
    SimFields(int size);

    // Public method
    void ClearFields();

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
        SimState();
        SimState(int N);
        SimState(int N, SimParams params);

        // Public methods
        void SetSources(float * density, float * xVelocity, float * yVelocity, float * temperature);
        void SimulationStep(float timeStep);
        void SetBoundaryClosed(bool isClosed);
        void ResetState();
        void ResetSources();
        void ResizeGrid(int N);

        // Array accessors
        float * GetDensity();
        float * GetXVelocity();
        float * GetYVelocity();
        float * GetTemperature();

        // Modified fields
        static float MixedDensity(int ind, SimParams params, SimFields fields);
        static float MixedDensityAtAirTemp(int ind, SimParams params, SimFields fields);
        static float MixedTemperature(int ind, SimParams params, SimFields fields);
        static float AdjustedMassDiffusivity(int ind, SimParams params, SimFields fields);
        static float AdjustedViscosity(int ind, SimParams params, SimFields fields);
        static float AdjustedThermalDiffusivity(int ind, SimParams params, SimFields fields);

        // Grid size accessors
        int GetN();
        int GetSize();

        // Parameter struct
        SimParams params;

        // Array struct
        SimFields fields;

    private:

        // Grid size
        int N;
        int size;

        // Internal Methods
        void SetSource(float *, float *);
        void SetConstantSource(float *, float);
        void AddSource(float *, float *, float);
        void AddHeatSource(float *, float *);
        void AddConstantSource(float *, float, float);

        void Diffuse(int b, float * x, float * x0, float (*diff)(int, SimParams, SimFields), float dt);
        void Dissipate(float *, float, float, float);
        void DissipateWithFallOff(float *, float, float, float, float);
        void Advect(int, float *, float *, float *, float *, float);
        void Convect(float *, float);

        void SetBoundary(int, float *);
        void HodgeProjection(float *, float *, float *, float *);

        void DensityStep(float);
        void VelocityStep(float);
        void TemperatureStep(float);
};

// Preprocessor close statement
#endif