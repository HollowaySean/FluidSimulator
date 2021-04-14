#include <iostream>

using namespace std;

// Macros
#define ind(i,j) ((i) + (N + 2)*(j))
#define swap(x0, x) {float *tmp = x0; x0 = x; x = tmp;}

// Function initializations
void addSource(int, float *, float *, float);
void diffuse(int, int, float *, float *, float, float);
void advect(int, int, float *, float *, float *, float *, float);
void densityStep(int, float *, float *, float *, float *, float, float);
void setBoundary(int, int, float *);
void displayGrid(int, float *);

int main()
{
    // Constant declarations
    int N = 10;
    int size = (N + 2) * (N + 2);

    // Array initializations
    float u[size], v[size], u_prev[size], v_prev[size];
     float dens[size], dens_prev[size];
     float densNew[size];

    float s[size];
    for(int i = 1; i < size; i++){ 
        s[i] = 0.0;
        dens[i] = 0.0;
        densNew[i] = 0.0;
    }
    s[ind(5,5)] = 1.0;

    densityStep(N, dens, s, u, v, 1, 0.1);
    
    displayGrid(N, dens);
    
    return 0;
}

void addSource(int N, float * x, float * s, float dt)
{
    int size = (N+2) * (N+2);

    // Loop through grid elements
    for(int i = 0; i < size; i++){
        x[i] += dt * s[i];
    }
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
            if(x < 0.5) { x = 0.5; }
            if(x > N + 0.5) {x = N + 0.5; }
            i0 = (int)x;
            i1 = i0 + 1;

            if(y < 0.5) { y = 0.5; }
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

void displayGrid(int N, float * x)
{
    int size = (N+2) * (N+2);
    string newChar;
    for(int i = 1; i < N + 1; i++){
        string str = "| ";
        for(int j = N; j > 0; j--){
            if(x[ind(i,j)] > 0.0){
                str += "x";
            }else{
                str += " ";
            }
            str += " ";
        }
        str += "|";
        cout << str << endl;
    }
}