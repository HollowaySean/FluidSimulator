
// Preprocessor statements
#ifndef SIMSOURCE_H
#define SIMSOURCE_H

// Include statements
#include <list>
#include "SimState.h"

// Structure which contains a density, velocity, or heat source for simulator
class SimSource
{
    public:

        // Constructor
        SimSource(SimState*);

        // Enumerable type designators
        enum Shape { square, circle, diamond, point };
        enum Type  { gas, wind, heat, energy };

        // SimState object
        SimState* simState;

        // Public methods
        void CreateGasSource(    Shape shape, float flowRate, float sourceTemp, float xCenter, float yCenter, float radius);
        void CreateWindSource(   float angle, float speed, float xCenter, float yCenter);
        void CreateHeatSource(   Shape shape, float sourceTemp, float xCenter, float yCenter, float radius);
        void CreateEnergySource( Shape shape, float flux, float referenceTemp, float referenceDensity, float xCenter, float yCenter, float radius);
        void CreateWindBoundary( float speed );

        // Update sim object
        void UpdateSources();

    protected:

        // Saved data from SimState
        int N;
        int size;
        float lengthScale;

        // Pointers to source grids
        float * xVel;
        float * yVel;
        float * dens;
        float * temp;

        // Sub-class for single source
        class Source
        {
            public:

                // Enumerable type designators
                Shape shape;
                Type type;

                // Size of source
                float radius;

                // Public methods
                void SetActive(bool isActive);

                // Active flag
                bool isActive = true;

                // Arrays of sources
                std::list<int> indices;
                float xVel;
                float yVel;
                float dens;
                float temp;

                // Index calculation method
                void SetIndices(int N, Shape shape, float xCenter, float yCenter, float radius);
        };

        class GasSource: public Source { 
            public:
                GasSource(int N, float lengthScale, Shape shape, float flowRate, float sourceTemp, 
                        float xCenter, float yCenter, float radius);
                float flowRate, sourceTemp; };

        class WindSource: public Source { 
            public:
                WindSource(int N, float lengthScale, float angle, float speed, 
                        float xCenter, float yCenter);
                float speed, direction; };

        class HeatSource: public Source { 
            public:
                HeatSource(int N, float lengthScale, Shape shape, float sourceTemp, 
                        float xCenter, float yCenter, float radius);
                float sourceTemp; };

        class EnergySource: public Source { 
            public:
                EnergySource(int N, float lengthScale, Shape shape, float flux, float referenceTemp, float referenceDensity,
                        float xCenter, float yCenter, float radius);
                float flux, referenceTemp; };

        class WindBoundary: public Source {
            public:
                WindBoundary(int N, float speed);
                float speed; };

        // List of sources
        std::list<SimSource::Source*> sources;
};

// Closing preprocessor statement
#endif