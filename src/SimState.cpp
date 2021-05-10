/* Function definition file for simulation state class */

// Include header definition
#include "SimState.h"

// Includes and usings
#include <iostream>
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
    this -> fields = SimFields(N);
}

// Constructor with physical properties
SimState::SimState(int N, float viscosity, float diffusion, float gravity, float airDensity)
{
    // Property initializations
    this -> N = N;
    this -> size = (N + 2) * (N + 2);

    // Struct initializations
    this -> params = SimParams(viscosity, diffusion, gravity, airDensity);
    this -> fields = SimFields(N);
}

// Set pointers to density and velocity sources
void SimState::SetSources(float * density, float * xVelocity, float * yVelocity)
{
    // Pass into struct
    fields.dens_source = density;
    fields.xVel_source = xVelocity;
    fields.yVel_source = yVelocity;
}

// Run simulation step
void SimState::SimulationStep(float timeStep)
{
    // Set sources as input
    SetSource(fields.dens_prev, fields.dens_source);
    SetSource(fields.xVel_prev, fields.xVel_source);
    SetSource(fields.yVel_prev, fields.yVel_source);

    // Start futher simulation steps
    VelocityStep(timeStep);
    DensityStep(timeStep);
}

// Property accessors
float * SimState::GetDensity() { return fields.dens; }
float * SimState::GetXVelocity() { return fields.xVel; }
float * SimState::GetYVelocity() { return fields.yVel; }
int SimState::GetN() { return N; }
int SimState::GetSize() { return size; }



//// SIMSTATE PRIVATE METHODS ////

// Set array values to those of other array
void SimState::SetSource(float * x, float * x_set)
{
    // Set array values at each cell
    for(int i = 0; i < size; i++){
        x[i] = x_set[i];
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
    for(int i = 1; i <= N; i++){
        x[ind(0,  i)] = (b == 1) ? -x[ind(1,i)] : x[ind(1,i)];
        x[ind(N+1,i)] = (b == 1) ? -x[ind(N,i)] : x[ind(N,i)];
        x[ind(i,  0)] = (b == 2) ? -x[ind(i,1)] : x[ind(i,1)];
        x[ind(i,N+1)] = (b == 2) ? -x[ind(i,N)] : x[ind(i,N)];
    }
    x[ind(0,    0)] = 0.5 * (x[ind(1,  0)] + x[ind(0,  1)]);
    x[ind(0,  N+1)] = 0.5 * (x[ind(1,N+1)] + x[ind(0,  N)]);
    x[ind(N+1,  0)] = 0.5 * (x[ind(N,  0)] + x[ind(N+1,1)]);
    x[ind(N+1,N+1)] = 0.5 * (x[ind(N,N+1)] + x[ind(N+1,N)]);
}

// Perform diffusion step
void SimState::Diffuse(int b, float * x, float * x0, float diff, float dt)
{
    // Adjust a to account for cell size
    // TODO:ADJUST
    float a = dt * diff * N * N;

    // Loop through Gauss-Seidel relaxation steps
    for(int k = 0; k < 20; k++){

        // Loop through grid elements
        for(int i = 1; i <= N; i++){
            for(int j = 1; j <= N; j++){

                // Diffusion step
                x[ind(i,j)] = (x0[ind(i,j)] + 
                a*(x[ind(i-1,j)] + x[ind(i+1,j)] + x[ind(i,j-1)] + x[ind(i,j+1)])) / (1 + 4*a);
            }
        }
        SetBoundary(b, x);
    }
}

// Perform advection step
void SimState::Advect(int b, float * d, float * d0, float * u, float * v, float dt)
{
    int i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1;
    float dt0 = dt * N;

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
    float h = 1.0/N;

    // Calculate divergence in each grid element 
    for(int i = 1; i <= N; i++){
        for(int j = 1; j <= N; j++){
            div[ind(i,j)] = -0.5 * h * (u[ind(i+1,j)]-u[ind(i-1,j)]+
                                        v[ind(i,j+1)]-v[ind(i,j-1)]);
            p[ind(i,j)] = 0;
        }
    }
    SetBoundary(0, div);
    SetBoundary(0, p);

    // Gauss-Seidel relaxation for divergence
    for(int k = 0; k < 20; k++){
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
            u[ind(i,j)] -= 0.5 * (p[ind(i+1,j)] - p[ind(i-1,j)])/h;
            v[ind(i,j)] -= 0.5 * (p[ind(i,j+1)] - p[ind(i,j-1)])/h;
        }
    }
    SetBoundary(1, u);
    SetBoundary(2, v);
}

void SimState::Gravitate(float * v, float * dens, float adens, float grav, float dt)
{
    float g = dt * grav;

    // Loop through grid elements
    for(int i = 1; i <= N; i++){
        for(int j = 1; j <= N; j++){

            // Gravitation and buoyancy
            v[ind(i,j)] += g * (dens[ind(i,j)] / (dens[ind(i,j)] + adens)); 
        }
    }
}

// Collected methods for density calculation
void SimState::DensityStep(float dt)
{
    // Perform source, diffusion, and advection steps
    AddSource(fields.dens, fields.dens_prev, dt);
    swap(fields.dens_prev, fields.dens); 
    Diffuse(0, fields.dens, fields.dens_prev, params.diff, dt);
    swap(fields.dens_prev, fields.dens); 
    Advect(0, fields.dens, fields.dens_prev, fields.xVel, fields.yVel, dt);
}

// Collected methods for velocity calculation
void SimState::VelocityStep(float dt)
{
    // Generate sources
    AddSource(fields.xVel, fields.xVel_prev, dt);
    AddSource(fields.yVel, fields.yVel_prev, dt);

    // Perform gravitational acceleration
    Gravitate(fields.xVel, fields.dens, params.airDens, params.grav, dt);

    // Perform velocity diffusion
    swap(fields.xVel_prev, fields.xVel);
    Diffuse(1, fields.xVel, fields.xVel_prev, params.visc, dt);
    swap(fields.yVel_prev, fields.yVel);
    Diffuse(2, fields.yVel, fields.yVel_prev, params.visc, dt);

    // Perform Hodge projection to remove divergence
    HodgeProjection(fields.xVel, fields.yVel, fields.xVel_prev, fields.yVel_prev);

    // Perform velocity advection
    swap(fields.xVel_prev, fields.xVel);
    swap(fields.yVel_prev, fields.yVel);
    Advect(1, fields.xVel, fields.xVel_prev, fields.xVel_prev, fields.yVel_prev, dt);
    Advect(2, fields.yVel, fields.yVel_prev, fields.xVel_prev, fields.yVel_prev, dt);

    // Perform Hodge projection again
    HodgeProjection(fields.xVel, fields.yVel, fields.xVel_prev, fields.yVel_prev);
}



/// STRUCT CONSTRUCTORS ///
SimParams::SimParams()
{
    // Set to static parameters
    visc = 0.;
    diff = 0.;
    grav = 0.;
    airDens = 0.;

    // Default options
    gravityOn = true;
    solverSteps = 20;
}

SimParams::SimParams(float viscosity, float diffusion, float gravity, float airDensity)
{
    // Set input parameters
    this -> visc = viscosity;
    this -> diff = diffusion;
    this -> grav = gravity;
    this -> airDens = airDensity;

    // Default options
    gravityOn = true;
    solverSteps = 20;

}

SimFields::SimFields()
{
    // Default to 10 x 10 grid
    SimFields(100);
}

SimFields::SimFields(int size)
{
    // Initialize all arrays
    xVel        = new float[size];
    yVel        = new float[size];
    dens        = new float[size];
    xVel_prev   = new float[size];
    yVel_prev   = new float[size];
    dens_prev   = new float[size];
    xVel_source = new float[size];
    yVel_source = new float[size];
    dens_source = new float[size];
}



/// EXTERNAL METHODS ///

// Display density on terminal
void DisplayGrid(SimState stateInput, float minimum)
{    

    // Get values from object
    int N = stateInput.GetN();
    float * x = stateInput.GetDensity();

    // Display on grid
    string str = "  ";
    for(int i = 1; i <= N; i++) { str += "--"; }
    cout << str << endl;

    for(int i = 1; i <= N; i++){
        str = "| ";
        for(int j = N; j >= 1; j--){
            if(x[ind(i,j)] > minimum){
                str += "x";
            }else{
                str += " ";
            }
            str += " ";
        }
        str += "|";
        cout << str << endl;
    }

    str = "  ";
    for(int i = 1; i <= N; i++){ str += "--"; }
    cout << str << endl;

}