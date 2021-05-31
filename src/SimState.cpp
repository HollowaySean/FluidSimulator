/* Function definition file for simulation state class */


// Include header definition
#include "headers/SimState.h"

// Includes and usings
#include <iostream>
#include <cmath>
using namespace std;

// Macros
#define ind(i,j) ((i) + (N + 2)*(j))
#define swap(x0, x) {float *tmp = x0; x0 = x; x = tmp;}



//// SIMSTATE PUBLIC METHODS ////

// Constructor without physical properties
SimState::SimState(int N)
{
    // Property initializations
    this -> N = N;
    this -> size = (N + 2) * (N + 2);

    // Struct initializations
    this -> params = SimParams();
    this -> fields = SimFields(size);

    // Zero out all arrays
    ResetState();
}

// Constructor taking param struct
SimState::SimState(int N, SimParams paramsIn)
{
    // Property initializations
    this -> N = N;
    this -> size = (N + 2) * (N + 2);

    // Struct initializations
    this -> params = paramsIn;
    this -> fields = SimFields(size);

    // Zero out all arrays
    ResetState();
}


// Set pointers to density and velocity sources
void SimState::SetSources(float * density, float * xVelocity, float * yVelocity, float * temperature)
{
    // Pass into struct
    fields.dens_source = density;
    fields.xVel_source = xVelocity;
    fields.yVel_source = yVelocity;
    fields.temp_source = temperature;
}

// Run simulation step
void SimState::SimulationStep(float timeStep)
{
    // Adjust for time scale
    float dt = timeStep * params.timeScale;

    // Set sources as input
    SetSource(fields.dens_prev, fields.dens_source);
    SetSource(fields.xVel_prev, fields.xVel_source);
    SetSource(fields.yVel_prev, fields.yVel_source);
    SetSource(fields.temp_prev, fields.temp_source);

    // Start futher simulation steps
    VelocityStep(dt);
    DensityStep(dt);
    if(params.temperatureOn)
        TemperatureStep(dt);
}

// Set boundaries open/closed
void SimState::SetBoundaryClosed(bool isClosed)
{
    params.closedBoundaries = isClosed;
}

// Reset to initial state of system
void SimState::ResetState()
{
    // Initialize arrays to 0
    SetConstantSource(fields.dens, 0.0);
    SetConstantSource(fields.xVel, 0.0);
    SetConstantSource(fields.yVel, 0.0);
    SetConstantSource(fields.temp, params.airTemp);
    SetConstantSource(fields.dens_prev, 0.0);
    SetConstantSource(fields.xVel_prev, 0.0);
    SetConstantSource(fields.yVel_prev, 0.0);
    SetConstantSource(fields.temp_prev, params.airTemp);
    SetConstantSource(fields.dens_source, 0.0);
    SetConstantSource(fields.xVel_source, 0.0);
    SetConstantSource(fields.yVel_source, 0.0);
    SetConstantSource(fields.temp_source, params.airTemp);
}

// Reset sources to initial state
void SimState::ResetSources()
{
    // Zero source arrays
    SetConstantSource(fields.dens_source, 0.0);
    SetConstantSource(fields.xVel_source, 0.0);
    SetConstantSource(fields.yVel_source, 0.0);
    SetConstantSource(fields.temp_source, params.airTemp);
}

// Modify grid parameters
void SimState::ResizeGrid(int N)
{
    // Property initializations
    this -> N = N;
    this -> size = (N + 2) * (N + 2);

    // Delete old field object and create new one
    fields.ClearFields();
    SimFields fields(size);
    this -> fields = fields;

    // Zero out all arrays
    ResetState();
}

// Property accessors
float * SimState::GetDensity() { return fields.dens; }
float * SimState::GetXVelocity() { return fields.xVel; }
float * SimState::GetYVelocity() { return fields.yVel; }
float * SimState::GetTemperature() { return fields.temp; }
int SimState::GetN() { return N; }
int SimState::GetSize() { return size; }

// Density field of mixed fluid at background temperature
float SimState::MixedDensityAtAirTemp(int ind, SimParams params, SimFields fields)
{
    return params.airDens + fields.dens[ind] * (1.0 - params.massRatio);
}

// Temperature field of mixed fluid
float SimState::MixedTemperature(int ind, SimParams params, SimFields fields)
{
    return params.airTemp + (fields.temp[ind] - params.airTemp) * (fields.dens[ind] / MixedDensityAtAirTemp(ind, params, fields));
}

// Density field of mixed fluid at temperature
float SimState::MixedDensity(int ind, SimParams params, SimFields fields)
{
    return MixedDensityAtAirTemp(ind, params, fields) * (params.airTemp / MixedTemperature(ind, params, fields));
}

// Mass diffusivity adjusted for temperature
float SimState::AdjustedMassDiffusivity(int ind, SimParams params, SimFields fields)
{
    return params.advancedCoefficients
            ? params.diff * sqrt(fields.temp[ind] / params.airTemp) * (fields.temp[ind] / params.airTemp)
            : params.diff;
}

// Viscosity adjusted for temperature
float SimState::AdjustedViscosity(int ind, SimParams params, SimFields fields)
{
    return params.advancedCoefficients
            ? params.visc * sqrt(MixedTemperature(ind, params, fields) / params.airTemp) / MixedDensityAtAirTemp(ind, params, fields)
            : params.visc;
}

// Thermal diffusivity adjusted for temperature
float SimState::AdjustedThermalDiffusivity(int ind, SimParams params, SimFields fields)
{
    return params.advancedCoefficients
            ? params.diffTemp * sqrt(fields.temp[ind] / params.airTemp)
            : params.diffTemp;
}


//// SIMSTATE PRIVATE METHODS ////

// Set array values to those of other array
void SimState::SetSource(float * x, float * x_set)
{
    // Set array values at each cell
    for(int i = 0; i < size; i++){
        x[i] = x_set[i];
    }
}

// Set array values to constant source value
void SimState::SetConstantSource(float * x, float x_set)
{
    // Set array values at each cell
    for(int i = 0; i < size; i++){
        x[i] = x_set;
    }
}

// Add source values into array values
void SimState::AddSource(float * x, float * s, float dt)
{
    // Loop through grid elements
    for(int i = 0; i < size; i++){
        x[i] += dt * s[i];
    }
}

// Add heat source via maximum temp (could use revision)
void SimState::AddHeatSource(float * t, float * s)
{
    // Loop through grid elements
    for(int i = 0; i < size; i++){
        t[i] = max(t[i], s[i]);
    }
}

// Add constant source value into array values
void SimState::AddConstantSource(float * x, float s, float dt)
{
    // Loop through grid elements
    for(int i = 0; i < size; i++){
        x[i] += dt * s;
    }
}

// Evaluate boundary conditions
void SimState::SetBoundary(int b, float * x)
{
    float xMod, yMod;

    switch(b){
        case -1: xMod =  0.; yMod =  0.; break;
        case  0: xMod =  1.; yMod =  1.; break;
        case  1: xMod = -1.; yMod =  1.; break;
        case  2: xMod =  1.; yMod = -1.; break;
    }

    for(int i = 1; i <= N; i++){
        x[ind(0,  i)] = xMod * x[ind(1,i)];
        x[ind(N+1,i)] = xMod * x[ind(N,i)];
        x[ind(i,  0)] = yMod * x[ind(i,1)];
        x[ind(i,N+1)] = yMod * x[ind(i,N)];
    }

    x[ind(0,    0)] = 0.5 * (x[ind(1,  0)] + x[ind(0,  1)]);
    x[ind(0,  N+1)] = 0.5 * (x[ind(1,N+1)] + x[ind(0,  N)]);
    x[ind(N+1,  0)] = 0.5 * (x[ind(N,  0)] + x[ind(N+1,1)]);
    x[ind(N+1,N+1)] = 0.5 * (x[ind(N,N+1)] + x[ind(N+1,N)]);
}

// Improved diffusion
void SimState::Diffuse(int b, float * x, float * x0, float (*diff)(int, SimParams, SimFields), float dt)
{
    // Adjust a to account for cell size and timestep
    float cellSize = params.lengthScale / N;
    float a = dt / (cellSize * cellSize);

    // Loop through Gauss-Seidel relaxation steps
    for(int k = 0; k < params.solverSteps; k++){

        // Loop through grid elements
        for(int i = 1; i <= N; i++){
            for(int j = 1; j <= N; j++){

                // Adjust for temperature and density using passed-in function
                float a_t = a * diff(ind(i,j), params, fields);

                // Diffusion step
                x[ind(i,j)] = (x0[ind(i,j)] + 
                a_t*(x[ind(i-1,j)] + x[ind(i+1,j)] + x[ind(i,j-1)] + x[ind(i,j+1)])) / (1 + 4*a_t);
            }
        }
        SetBoundary(b, x);
    }
}

// Dissipate density
void SimState::Dissipate(float * x, float eqVal, float rate, float dt)
{
    // Adjust for time scale
    float d = rate * dt;

    // Loop through grid elements
    for(int i = 0; i < size; i++){

        // Decay temperatures
        x[i] -= d * (x[i] - eqVal);
    }
}

// Dissipate density based on temperature
void SimState::DissipateWithFallOff(float * x, float eqVal, float rate, float fallOff, float dt)
{
    // Adjust for time scale
    float d = rate * dt;

    // Loop through grid elements
    for(int i = 0; i < size; i++){

        // Decay temperatures
        x[i] -= d * (1. - fallOff * (fields.temp[i] - params.airTemp)) * (x[i] - eqVal) ;
    }
}

// Perform advection step
void SimState::Advect(int b, float * d, float * d0, float * u, float * v, float dt)
{
    int i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1;
    
    // Adjust dt to account for cell size
    float cellSize = params.lengthScale / N;
    float dt0 = dt / cellSize;

    // Loop through grid elements
    for(int i = 1; i <= N; i++){
        for(int j = 1; j <= N; j++){

            // Calculate origin coordinates
            x = i - dt0 * u[ind(i,j)];
            y = j - dt0 * v[ind(i,j)];

            // Discretize into adjacent grid elements
            if(x <     0.5) { x =     0.5; }
            if(x > N + 0.5) { x = N + 0.5; }
            i0 = (int)x;
            i1 = i0 + 1;

            if(y <     0.5) { y =     0.5; }
            if(y > N + 0.5) { y = N + 0.5; }
            j0 = (int)y;
            j1 = j0 + 1;

            s1 = x - i0;
            s0 = 1 - s1;
            t1 = y - j0;
            t0 = 1 - t1;

            // Calculate new value due to advection
            d[ind(i,j)] = s0 * (t0 * d0[ind(i0,j0)] + t1 * d0[ind(i0,j1)]) +
                          s1 * (t0 * d0[ind(i1,j0)] + t1 * d0[ind(i1,j1)]);
        }
    }
    SetBoundary(b, d);
}

// Perform Hodge Projection for advection
void SimState::HodgeProjection(float * u, float * v, float * p, float * div)
{
    // Adjust for cell size
    float cellSize = params.lengthScale / N;

    // Calculate divergence in each grid element 
    for(int i = 1; i <= N; i++){
        for(int j = 1; j <= N; j++){
            div[ind(i,j)] = -0.5 * cellSize * (u[ind(i+1,j)]-u[ind(i-1,j)]+
                                        v[ind(i,j+1)]-v[ind(i,j-1)]);
            p[ind(i,j)] = 0;
        }
    }
    SetBoundary(0, div);
    SetBoundary(0, p);

    // Gauss-Seidel relaxation for divergence
    for(int k = 0; k < params.solverSteps; k++){
        for(int i = 1; i <= N; i++){
            for(int j = 1; j <= N; j++){
                p[ind(i,j)] = (div[ind(i,j)] + p[ind(i-1,j)] + p[ind(i+1,j)] +
                                               p[ind(i,j-1)] + p[ind(i,j+1)])/4;
            }
        }
        SetBoundary(0, p);
    }

    // Calculate divergence-free Hodge projection in each grid element 
    for(int i = 1; i <= N; i++){
        for(int j = 1; j <= N; j++){
            u[ind(i,j)] -= 0.5 * (p[ind(i+1,j)] - p[ind(i-1,j)]) / cellSize;
            v[ind(i,j)] -= 0.5 * (p[ind(i,j+1)] - p[ind(i,j-1)]) / cellSize;
        }
    }
    SetBoundary(1, u);
    SetBoundary(2, v);
}

// Perform thermal and gravitational convection
void SimState::Convect(float * v, float dt)
{
    // Adjust for time scale
    float g = dt * params.grav;

    // Loop through grid elements
    for(int i = 1; i <= N; i++){
        for(int j = 1; j <= N; j++){

            // Calculate thermal buoyancy values
            float density;
            if(params.temperatureOn){
                density = MixedDensity(ind(i,j), params, fields);
            }else{
                density = MixedDensityAtAirTemp(ind(i,j), params, fields);
            }

            // Calculate buoyant force
            float bForce;
            if(density == 0.0){
                bForce = 1.0;
            }else{
                bForce = (density - params.airDens) / density;
            }

            // Apply force to stream vector
            v[ind(i,j)] += g * bForce;
        }
    }
}

// Collected methods for density calculation
void SimState::DensityStep(float dt)
{
    // Add density source
    AddSource(fields.dens, fields.dens_prev, dt);

    // Diffuse by Fick's law
    swap(fields.dens_prev, fields.dens); 
    Diffuse(params.closedBoundaries ? 0 : -1, fields.dens, fields.dens_prev, SimState::AdjustedMassDiffusivity, dt);
    swap(fields.dens_prev, fields.dens); 

    // Dissipate smoke
    if(params.densDecay > 0.0){
        if(params.tempFactor > 0.0){
            DissipateWithFallOff(fields.dens, 0.0, params.densDecay, params.tempFactor, dt);
        }else{
            Dissipate(fields.dens, 0.0, params.densDecay, dt);
        }
    }

    // Advect along streamlines
    Advect(params.closedBoundaries ? 0 : -1, fields.dens, fields.dens_prev, fields.xVel, fields.yVel, dt);
}

// Collected methods for velocity calculation
void SimState::VelocityStep(float dt)
{
    // Generate sources
    AddSource(fields.xVel, fields.xVel_prev, dt);
    AddSource(fields.yVel, fields.yVel_prev, dt);

    // Perform gravitational acceleration
    if(params.gravityOn && params.grav != 0.0){
        Convect(fields.yVel, dt);
    }

    // Perform velocity diffusion
    swap(fields.xVel_prev, fields.xVel);
    Diffuse(params.closedBoundaries ? 1 : 0, fields.xVel, fields.xVel_prev, SimState::AdjustedViscosity, dt);
    swap(fields.yVel_prev, fields.yVel);
    Diffuse(params.closedBoundaries ? 2 : 0, fields.yVel, fields.yVel_prev, SimState::AdjustedViscosity,dt);

    // Perform Hodge projection to remove divergence
    HodgeProjection(fields.xVel, fields.yVel, fields.xVel_prev, fields.yVel_prev);

    // Perform velocity advection
    swap(fields.xVel_prev, fields.xVel);
    swap(fields.yVel_prev, fields.yVel);
    Advect(params.closedBoundaries ? 1 : 0, fields.xVel, fields.xVel_prev, fields.xVel_prev, fields.yVel_prev, dt);
    Advect(params.closedBoundaries ? 2 : 0, fields.yVel, fields.yVel_prev, fields.xVel_prev, fields.yVel_prev, dt);

    // Perform Hodge projection again
    HodgeProjection(fields.xVel, fields.yVel, fields.xVel_prev, fields.yVel_prev);
}

// Collected methods for temperature calculation
void SimState::TemperatureStep(float dt)
{
    // Generate sources
    AddHeatSource(fields.temp, fields.temp_prev);

    // Perform thermal diffusion
    swap(fields.temp_prev, fields.temp);
    Diffuse(0, fields.temp, fields.temp_prev, SimState::AdjustedThermalDiffusivity, dt);
    swap(fields.temp_prev, fields.temp);

    // Perform cooling due to surrounding air
    if(params.tempDecay > 0.0){
        Dissipate(fields.temp, params.airTemp, params.tempDecay, dt);
    }

    // Advect along streamlines
    Advect(0, fields.temp, fields.temp_prev, fields.xVel, fields.yVel, dt);
}



/// STRUCT CONSTRUCTORS ///

// Bare constructor
SimParams::SimParams()
{
    // Set to static parameters
    lengthScale = 1.;
    timeScale = 1.;
    visc = 0.;
    diff = 0.;
    grav = 0.;
    airDens = 0.;
    massRatio = 0.;
    airTemp = 0.;
    diffTemp = 0.;
    densDecay = 0.;
    tempFactor = 0.;
    tempDecay = 0.;

    // Default options
    closedBoundaries = true;
    gravityOn = false;
    temperatureOn = false;
    advancedCoefficients = false;
    solverSteps = 20;
}

// Constructor for simple advection/diffusion simulation
SimParams::SimParams(float lengthScale, float timeScale, float viscosity, float diffusion)
{
    // Set input parameters
    this -> lengthScale = lengthScale;
    this -> timeScale = timeScale;
    this -> visc = viscosity;
    this -> diff = diffusion;

    // Set static parameters otherwise
    grav = 0.;
    airDens = 0.;
    massRatio = 0.;
    airTemp = 0.;
    diffTemp = 0.;
    densDecay = 0.;
    tempFactor = 0.;
    tempDecay = 0.;

    // Default options
    closedBoundaries = true;
    gravityOn = false;
    temperatureOn = false;
    advancedCoefficients = false;
    solverSteps = 20;

}

// Constructor for buoyant simulation
SimParams::SimParams(float lengthScale, float timeScale, float viscosity, float diffusion, 
                     float gravity, float airDensity, float massRatio)
{
    // Set input parameters
    this -> lengthScale = lengthScale;
    this -> timeScale = timeScale;
    this -> visc = viscosity;
    this -> diff = diffusion;
    this -> grav = gravity;
    this -> airDens = airDensity;
    this -> massRatio = massRatio;

    // Set static parameters otherwise
    airTemp = 0.;
    diffTemp = 0.;
    densDecay = 0.;
    tempFactor = 0.;
    tempDecay = 0.;

    // Default options
    closedBoundaries = true;
    gravityOn = true;
    temperatureOn = false;
    advancedCoefficients = false;
    solverSteps = 20;

}

// Constructor for full thermal simulation
SimParams::SimParams(float lengthScale, float timeScale, float viscosity, float diffusion, 
                     float gravity, float airDensity, float massRatio, 
                     float airTemp, float diffTemp)
{
    // Set input parameters
    this -> lengthScale = lengthScale;
    this -> timeScale = timeScale;
    this -> visc = viscosity;
    this -> diff = diffusion;
    this -> grav = gravity;
    this -> airDens = airDensity;
    this -> massRatio = massRatio;
    this -> airTemp = airTemp;
    this -> diffTemp = diffTemp;

    // Set static parameters otherwise
    densDecay = 0.;
    tempFactor = 0.;
    tempDecay = 0.;

    // Default options
    closedBoundaries = true;
    gravityOn = true;
    temperatureOn = true;
    advancedCoefficients = true;
    solverSteps = 20;

}

// Constructor for full thermal simulation
SimParams::SimParams(float lengthScale, float timeScale, float viscosity, float diffusion, 
                     float gravity, float airDensity, float massRatio, 
                     float airTemp, float diffTemp, 
                     float densDecay, float tempFactor, float tempDecay)
{
    // Set input parameters
    this -> lengthScale = lengthScale;
    this -> timeScale = timeScale;
    this -> visc = viscosity;
    this -> diff = diffusion;
    this -> grav = gravity;
    this -> airDens = airDensity;
    this -> massRatio = massRatio;
    this -> airTemp = airTemp;
    this -> diffTemp = diffTemp;
    this -> densDecay = densDecay;
    this -> tempFactor = tempFactor;
    this -> tempDecay = tempDecay;

    // Default options
    closedBoundaries = true;
    gravityOn = true;
    temperatureOn = true;
    advancedCoefficients = true;
    solverSteps = 20;
}

// Return pointer to float by index
float* SimParams::FloatPointer(int paramNum, ParamType type)
{
    float* points[4][4]
    {
        {
            &lengthScale,
            &timeScale
        },
        {
            &visc,
            &diff,
            &diffTemp
        },
        {
            &grav,
            &airDens,
            &massRatio,
            &airTemp
        },
        {
            &densDecay,
            &tempFactor,
            &tempDecay
        }
    };

    return points[type][paramNum];
}

// Return name of parameter by index
std::string SimParams::FloatName(int paramNum, ParamType type)
{
    std::string names[4][4]
    {
        {
            "Length Scale",
            "Time Scale"
        },
        {
            "Viscosity",
            "Molecular Diffusion",
            "Thermal Diffusion"
        },
        {
            "Gravitational Force",
            "Background Density",
            "Mass Ratio",
            "Background Temperature"
        },
        {
            "Density Decay Rate",
            "Decay Temperature Factor",
            "Temperature Decay Rate"
        }
    };

    return names[type][paramNum];
}

// Return tooltip string of parameter by index
std::string SimParams::FloatTip(int paramNum, ParamType type)
{
    std::string tips[4][4]
    {
        {
            "Length Scale (m): \nWidth of the simulation space",
            "Time Scale (unitless): \nSeconds of simulation time per second of real time"
        },
        {
            "Viscosity (m^2/s): \nKinematic viscosity at background temperature and standard pressure",
            "Molecular Diffusion (m^2/s): \nDiffusivity at background temperature and standard pressure",
            "Thermal Diffusion (m^2/s): \nThermal diffusivity at background temperature and standard pressure"
        },
        {
            "Gravitational Force (m/s^2): \nGravitational acceleration",
            "Background Density (kg/m^3): \nDensity of background medium",
            "Mass Ratio (unitless): \nMolar mass of background medium divided by molar mass of fluid",
            "Background Temperature (K): \nTemperature of background medium"
        },
        {
            "Density Decay Rate (1/s): \nRate at which fluid goes proportionally to zero",
            "Decay Temperature Factor (1/K): \nIncrease in density decay per degree above background temperature",
            "Temperature Decay Rate (1/s): \nRate at which temperature goes proportionally to the background temperature"
        }
    };

    return tips[type][paramNum];
}

// Return minimum for slider
float SimParams::FloatMin(int paramNum, ParamType type)
{
    float mins[4][4]
    {
        {
            0.001,
            0.0
        },
        {
            0.000001,
            0.000001,
            0.000001
        },
        {
            -100.0,
            0.0,
            0.0,
            1.0
        },
        {
            0.0,
            0.0,
            0.0
        }
    };

    return mins[type][paramNum];
}

// Return maximum for slider
float SimParams::FloatMax(int paramNum, ParamType type)
{
    float maxs[4][4]
    {
        {
            100.0,
            100.0
        },
        {
            0.001,
            0.001,
            0.001
        },
        {
            100.0,
            1000.0,
            1000.0,
            10000.0
        },
        {
            1000.0,
            1000.0,
            1000.0
        }
    };

    return maxs[type][paramNum];
}

// Field object constructor, bare
SimFields::SimFields()
{
    // Default to 10 x 10 grid
    SimFields(100);
}

// Field object constructor
SimFields::SimFields(int size)
{
    // Initialize all arrays
    xVel          = new float[size];
    yVel          = new float[size];
    dens          = new float[size];
    temp          = new float[size];
    xVel_prev     = new float[size];
    yVel_prev     = new float[size];
    dens_prev     = new float[size];
    temp_prev     = new float[size];
    xVel_source   = new float[size];
    yVel_source   = new float[size];
    dens_source   = new float[size];
    temp_source   = new float[size];
}

// Delete field arrays
void SimFields::ClearFields()
{
    // Delete all arrays
    delete[] xVel;
    delete[] yVel;
    delete[] dens;
    delete[] temp;
    delete[] xVel_prev;
    delete[] yVel_prev;
    delete[] dens_prev;
    delete[] temp_prev;
    delete[] xVel_source;
    delete[] yVel_source;
    delete[] dens_source;
    delete[] temp_source;
}
