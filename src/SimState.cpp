/* Function definition file for simulation state class */

// Include header definition
#include "SimState.h"

// Includes and usings
#include <iostream>
using namespace std;

// Macros
#define ind(i,j) ((i) + (N + 2)*(j))
#define swap(x0, x) {float *tmp = x0; x0 = x; x = tmp;}



//// PUBLIC METHODS ////

// Constructor without physical properties
SimState::SimState(int N)
{
    // Property initializations
    this -> N = N;
    this -> size = (N + 2) * (N + 2);

    // Array initializations
    u           = new float[size];
    v           = new float[size];
    dens        = new float[size];
    u_prev      = new float[size];
    v_prev      = new float[size];
    dens_prev   = new float[size];
    u_source    = new float[size];
    v_source    = new float[size];
    dens_source = new float[size];
}

// Constructor with physical properties
SimState::SimState(int N, float visc, float diff, float grav, float airDensity)
{
    // Property initializations
    this -> N = N;
    this -> size = (N + 2) * (N + 2);
    this -> visc = visc;
    this -> diff = diff;
    this -> grav = grav;
    this -> airDensity = airDensity;

    // Array initializations
    u           = new float[size];
    v           = new float[size];
    dens        = new float[size];
    u_prev      = new float[size];
    v_prev      = new float[size];
    dens_prev   = new float[size];
    u_source    = new float[size];
    v_source    = new float[size];
    dens_source = new float[size];
}

// Set pointers to density and velocity sources
void SimState::SetSources(float * densitySource, float * uSource, float * vSource)
{
    this -> dens_source = densitySource;
    this -> u_source = uSource;
    this -> v_source = vSource;
}

// Run simulation step
void SimState::SimulationStep(float timeStep)
{
    // Set sources as input
    SetSource(N, dens_prev, dens_source);
    SetSource(N, u_prev,    u_source);
    SetSource(N, v_prev,    v_source);

    // Start futher simulation steps
    VelocityStep(N, u, v, u_prev, v_prev, dens, airDensity, visc, grav, timeStep);
    DensityStep(N, dens, dens_prev, u, v, diff, timeStep);
}

// Property accessors
float * SimState::GetDensity() { return dens; }
float * SimState::GetUVelocity() { return u; }
float * SimState::GetVVelocity() { return v; }
int SimState::GetN() { return N; }
int SimState::GetSize() { return size; }



//// PRIVATE METHODS ////

// Set array values to those of other array
void SimState::SetSource(int N, float * x, float * x_set)
{
    // Set array values at each cell
    for(int i = 0; i <= N+1; i++){
        for(int j = 0; j <= N+1; j++){
            x[ind(i,j)] = x_set[ind(i,j)];
        }
    }
}

// Add source values into array values
void SimState::AddSource(int N, float * x, float * s, float dt)
{
    int size = (N+2) * (N+2);

    // Loop through grid elements
    for(int i = 0; i < size; i++){
        x[i] += dt * s[i];
    }
}

// Add constant source value into array values
void SimState::AddConstantSource(int N, float * x, float s, float dt)
{
    int size = (N+2) * (N+2);

    // Loop through grid elements
    for(int i = 0; i < size; i++){
        x[i] += dt * s;
    }
}

// Evaluate boundary conditions
void SimState::SetBoundary(int N, int b, float * x)
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
void SimState::Diffuse(int N, int b, float * x, float * x0, float diff, float dt)
{
    int i, j, k;
    float a = dt * diff * N * N;

    // Loop through Gauss-Seidel relaxation steps
    for(k = 0; k < 20; k++){

        // Loop through grid elements
        for(i = 1; i <= N; i++){
            for(j = 1; j <= N; j++){

                // Diffusion step
                x[ind(i,j)] = (x0[ind(i,j)] + 
                a*(x[ind(i-1,j)] + x[ind(i+1,j)] + x[ind(i,j-1)] + x[ind(i,j+1)])) / (1 + 4*a);
            }
        }
        SetBoundary(N, b, x);
    }
}

// Perform advection step
void SimState::Advect(int N, int b, float * d, float * d0, float * u, float * v, float dt)
{
    int i, j, i0, j0, i1, j1;
    float x, y, s0, t0, s1, t1;
    float dt0 = dt * N;

    // Loop through grid elements
    for(i = 1; i <= N; i++){
        for(j = 1; j <= N; j++){

            // Calculate origin coordinates
            x = i - dt0 * u[ind(i,j)];
            y = j - dt0 * v[ind(i,j)];

            // Discretize into adjacent grid elements
            if(x < 0.5    ) { x = 0.5;     }
            if(x > N + 0.5) { x = N + 0.5; }
            i0 = (int)x;
            i1 = i0 + 1;

            if(y < 0.5    ) { y = 0.5;     }
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
    SetBoundary(N, b, d);
}

// Perform Hodge Projection for advection
void SimState::HodgeProjection(int N, float * u, float * v, float * p, float * div)
{
    int i, j, k;
    float h = 1.0/N;

    // Calculate divergence in each grid element 
    for(i = 1; i <= N; i++){
        for(j = 1; j <= N; j++){
            div[ind(i,j)] = -0.5 * h * (u[ind(i+1,j)]-u[ind(i-1,j)]+
                                        v[ind(i,j+1)]-v[ind(i,j-1)]);
            p[ind(i,j)] = 0;
        }
    }
    SetBoundary(N, 0, div);
    SetBoundary(N, 0, p);

    // Gauss-Seidel relaxation for divergence
    for(k = 0; k < 20; k++){
        for(i = 1; i <= N; i++){
            for(j = 1; j <= N; j++){
                p[ind(i,j)] = (div[ind(i,j)] + p[ind(i-1,j)] + p[ind(i+1,j)] +
                                               p[ind(i,j-1)] + p[ind(i,j+1)])/4;
            }
        }
        SetBoundary(N, 0, p);
    }

    // Calculate divergence-free Hodge projection in each grid element 
    for(i = 1; i <= N; i++){
        for(j = 1; j <= N; j++){
            u[ind(i,j)] -= 0.5 * (p[ind(i+1,j)] - p[ind(i-1,j)])/h;
            v[ind(i,j)] -= 0.5 * (p[ind(i,j+1)] - p[ind(i,j-1)])/h;
        }
    }
    SetBoundary(N, 1, u);
    SetBoundary(N, 2, v);
}

void SimState::Gravitate(int N, float * v, float * dens, float adens, float grav, float dt)
{
    int i, j;
    float g = dt * grav;

    // Loop through grid elements
    for(i = 1; i <= N; i++){
        for(j = 1; j <= N; j++){

            // Gravitation and buoyancy
            v[ind(i,j)] += g * ((dens[ind(i,j)] - adens) / (dens[ind(i,j)] + adens)); 
        }
    }
}

// Collected methods for density calculation
void SimState::DensityStep(int N, float * x, float * x0, float * u, float * v, float diff, float dt)
{
    // Perform source, diffusion, and advection steps
    AddSource(N, x, x0, dt);
    swap(x0, x); Diffuse(N, 0, x, x0, diff, dt);
    swap(x0, x); Advect(N, 0, x, x0, u, v, dt);
}

// Collected methods for velocity calculation
void SimState::VelocityStep(int N, float * u, float * v, float * u0, float * v0, float * dens, float adens, float visc, float grav, float dt)
{
    // Generate sources
    AddSource(N, u, u0, dt);
    AddSource(N, v, v0, dt);

    // Perform gravitational acceleration
    Gravitate(N, v, dens, adens, grav, dt);

    // Perform velocity diffusion
    swap(u0, u);
    Diffuse(N, 1, u, u0, visc, dt);
    swap(v0, v);
    Diffuse(N, 2, v, v0, visc, dt);

    // Perform Hodge projection to remove divergence
    HodgeProjection(N, u, v, u0, v0);

    // Perform velocity advection
    swap(u0, u);
    swap(v0, v);
    Advect(N, 1, u, u0, u0, v0, dt);
    Advect(N, 2, v, v0, u0, v0, dt);

    // Perform Hodge projection again
    HodgeProjection(N, u, v, u0, v0);
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