/* Header file for simulation state class */

class SimState
{
    public:

        // Constructors
        SimState(int N);
        SimState(int N, float visc, float diff);

        // Public methods
        void SetSources(float * density, float * uVelocity, float * vVelocity);
        void SimulationStep(float timeStep);

        // Array accessors
        float * GetDensity();
        float * GetUVelocity();
        float * GetVVelocity();

        // Grid size accessors
        int GetN();
        int GetSize();

        // Physical properties
        float visc;
        float diff;

    private:

        // Grid size
        int N;
        int size;

        // Internal Methods
        void SetSource(int, float *, float *);
        void AddSource(int, float *, float *, float);
        void SetBoundary(int, int, float *);
        void Diffuse(int, int, float *, float *, float, float);
        void Advect(int, int, float *, float *, float *, float *, float);
        void DensityStep(int, float *, float *, float *, float *, float, float);
        void HodgeProjection(int, float *, float *, float *, float *);
        void VelocityStep(int, float *, float *, float *, float *, float, float);

        /// Arrays: ///
        // Current grid
        float * u;
        float * v;
        float * dens;

        // Previous grid
        float * u_prev;
        float * v_prev;
        float * dens_prev;

        // Source grid
        float * u_source;
        float * v_source;
        float * dens_source;
};

// Method to display simulation state in terminal
void DisplayGrid(SimState stateInput, float minimum);