#include <iostream>
#include <GL/glut.h>

using namespace std;

// Macros
#define ind(i,j) ((i) + (N + 2)*(j))
#define swap(x0, x) {float *tmp = x0; x0 = x; x = tmp;}

// Function initializations
void setSource(int, float *, float *);
void addSource(int, float *, float *, float);
void setBoundary(int, int, float *);
void diffuse(int, int, float *, float *, float, float);
void advect(int, int, float *, float *, float *, float *, float);
void densityStep(int, float *, float *, float *, float *, float, float);
void hodgeProjection(int, float *, float *, float *, float *);
void velocityStep(int, float *, float *, float *, float *, float, float);
void displayGrid(int, float *, float);

int main(int argc, char** argv)
{
    // Declarations
    int N = 25;
    int size = (N + 2) * (N + 2);
    int numIterations = 10;
    float visc = 10;
    float diff = 10;
    float dt = 0.01;

    // Array initializations
    float u[size]         = { 0 };
    float v[size]         = { 0 };
    float u_prev[size]    = { 0 };
    float v_prev[size]    = { 0 };
    float dens[size]      = { 0 };
    float dens_prev[size] = { 0 };

    float dens_source[size] = { 0 };
    float u_source[size] = { 0 };
    float v_source[size] = { 0 };

    // Set conditions
    for(int i = 0; i <= N+1; i++){
        for(int j = 0; j <= N+1; j++){
            u_source[ind(i,j)] = 100.0;
            v_source[ind(i,j)] = 100.0;
        }
    }
    dens_source[ind(13,13)] = 100.0;

    // Run simulation and display loop
    primaryLoop();

    return 0;
}

class State {
    public:

        // Parameters
        const int N;
        const int size;
        float visc;
        float diff;

        // Array pointers
        const float *u;
        const float *v;
        const float *dens;
        const float *u_prev;
        const float *v_prev;
        const float *dens_prev;
        const float *u_source;
        const float *v_source;
        const float *dens_source;

};

void primaryLoop(){

    // // Simulation loop
    // while(true)
    // {
    //     // Set sources
    //     setSource(N, dens_prev, dens_source);
    //     setSource(N, u_prev, u_source);
    //     setSource(N, v_prev, v_source);

    //     // Simulation
    //     velocityStep(N, u, v, u_prev, v_prev, visc, dt);
    //     densityStep(N, dens, dens_prev, u, v, diff, dt);
    //     displayGrid(N, dens, 0.1);

    //     // Wait for keyboard input
    //     cin.get();
    // }
}

void setSource(int N, float * x, float * x_set)
{
    // Set array values at each cell
    for(int i = 0; i <= N+1; i++){
        for(int j = 0; j <= N+1; j++){
            x[ind(i,j)] = x_set[ind(i,j)];
        }
    }
}

void addSource(int N, float * x, float * s, float dt)
{
    int size = (N+2) * (N+2);

    // Loop through grid elements
    for(int i = 0; i < size; i++){
        x[i] += dt * s[i];
    }
}

void setBoundary(int N, int b, float * x)
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

void diffuse(int N, int b, float * x, float * x0, float diff, float dt)
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
        setBoundary(N, b, x);
    }
}

void advect(int N, int b, float * d, float * d0, float * u, float * v, float dt)
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
    setBoundary(N, b, d);
}

void densityStep(int N, float * x, float * x0, float * u, float * v, float diff, float dt)
{
    // Perform source, diffusion, and advection steps
    addSource(N, x, x0, dt);
    swap(x0, x); diffuse(N, 0, x, x0, diff, dt);
    swap(x0, x); advect(N, 0, x, x0, u, v, dt);
}

void hodgeProjection(int N, float * u, float * v, float * p, float * div)
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
    setBoundary(N, 0, div);
    setBoundary(N, 0, p);

    // Gauss-Seidel relaxation for divergence
    for(k = 0; k < 20; k++){
        for(i = 1; i <= N; i++){
            for(j = 1; j <= N; j++){
                p[ind(i,j)] = (div[ind(i,j)] + p[ind(i-1,j)] + p[ind(i+1,j)] +
                                               p[ind(i,j-1)] + p[ind(i,j+1)])/4;
            }
        }
        setBoundary(N, 0, p);
    }

    // Calculate divergence-free Hodge projection in each grid element 
    for(i = 1; i <= N; i++){
        for(j = 1; j <= N; j++){
            u[ind(i,j)] -= 0.5 * (p[ind(i+1,j)] - p[ind(i-1,j)])/h;
            v[ind(i,j)] -= 0.5 * (p[ind(i,j+1)] - p[ind(i,j-1)])/h;
        }
    }
    setBoundary(N, 1, u);
    setBoundary(N, 2, v);
}

void velocityStep(int N, float * u, float * v, float * u0, float * v0, float visc, float dt)
{
    // Generate sources
    addSource(N, u, u0, dt);
    addSource(N, v, v0, dt);

    // Perform velocity diffusion
    swap(u0, u);
    diffuse(N, 1, u, u0, visc, dt);
    swap(v0, v);
    diffuse(N, 2, v, v0, visc, dt);

    // Perform Hodge projection to remove divergence
    hodgeProjection(N, u, v, u0, v0);

    // Perform velocity advection
    swap(u0, u);
    swap(v0, v);
    advect(N, 1, u, u0, u0, v0, dt);
    advect(N, 2, v, v0, u0, v0, dt);

    // Perform Hodge projection again
    hodgeProjection(N, u, v, u0, v0);

}

void displayGrid(int N, float * x, float min)
{
    int size = (N+2) * (N+2);
    
    string str = "  ";
    for(int i = 1; i <= N; i++) { str += "--"; }
    cout << str << endl;

    for(int i = 1; i <= N; i++){
        str = "| ";
        for(int j = N; j >= 1; j--){
            if(x[ind(i,j)] > min){
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