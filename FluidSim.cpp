#include <iostream>

using namespace std;

#define ind(i,j) ((i) + (N + 2)*(j))

void addSource(int, float *, float *, float);
void displayGrid(int, float *);

int main()
{
    // Constant declarations
    const int N = 10;
    const int gridSize = (N + 2) * (N + 2);

    // Array initializations
    static float u[gridSize], v[gridSize], u_prev[gridSize], v_prev[gridSize];
    static float dens[gridSize], dens_prev[gridSize];

    float s[gridSize];
    for(int i = 1; i < gridSize; i++){ 
        s[i] = 0.0;
    }
    s[ind(5,5)] = 1.0;
    
    displayGrid(N, s);
    return 0;
}

void addSource(int N, float * x, float * s, float dt)
{
    int size = (N+2) * (N+2);
    for(int i = 0; i < size; i++){
        x[i] += dt * s[i];
    }
}

void displayGrid(int N, float * x)
{
    int size = (N+2) * (N+2);
    string newChar;
    for(int i = 1; i < N; i++){
        string str = "| ";
        for(int j = 1; j < N; j++){
            if(x[ind(i,j)] > 0.5){
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