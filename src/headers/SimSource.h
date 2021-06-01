
// Preprocessor statements
#ifndef SIMSOURCE_H
#define SIMSOURCE_H

// Include statements
#include <list>
#include <random>
#include "SimState.h"

// Structure which contains a density, velocity, or heat source for simulator
class SimSource
{
    public:

        // Constructor
        SimSource(SimState*);

        // Enumerable type designators
        enum Shape { square, circle, diamond, point };
        enum Type  { gas, wind, windBoundary, heat, energy };

        // SimState object
        SimState* simState;

        // Public methods
        void CreateGasSource(   Shape shape, float flowRate, float sourceTemp, 
                                float xCenter, float yCenter, float radius);
        void CreateGasSourceDynamic(    
                                Shape shape, float flowRate, float sourceTemp, 
                                float xCenter, float yCenter, float radius, 
                                float flowVar, float tempVar);
        void CreateWindSource(  float angle, float speed, 
                                float xCenter, float yCenter);
        void CreateWindSourceDynamic(   
                                float angle, float speed, 
                                float xCenter, float yCenter,
                                float speedVar, float angleVar);
        void CreateHeatSource(  Shape shape, float sourceTemp, 
                                float xCenter, float yCenter, float radius);
        void CreateHeatSourceDynamic(  
                                Shape shape, float sourceTemp, 
                                float xCenter, float yCenter, float radius,
                                float tempVar);
        void CreateEnergySource(Shape shape, float flux, float referenceTemp, float referenceDensity, 
                                float xCenter, float yCenter, float radius);
        void CreateEnergySourceDynamic(
                                Shape shape, float flux, float referenceTemp, float referenceDensity, 
                                float xCenter, float yCenter, float radius,
                                float fluxVar);
        void CreateWindBoundary(float speed );
        void CreateWindBoundaryDynamic(
                                float speed, float speedVar);

        void RemoveSourceAtPoint(float x, float y, float dist);
        void RemoveAllSources();

        // Update sim object
        void UpdateSources();
        void UpdateSourcesDynamic();
        void Reset();

    protected:

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
                float xCenter;
                float yCenter;

                // Public methods
                void SetActive(bool isActive);

                // Flags
                bool isActive = true;
                bool isDynamic = false;

                // Arrays of sources
                std::list<int> indices;
                float xVel;
                float yVel;
                float dens;
                float temp;

                // Dynamic properties
                float wVar = 0.0;
                float aVar = 0.0;
                float wMean = 0.0;
                float aMean = 0.0;
                float dVar = 0.0;
                float tVar = 0.0;

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

        // Protected methods
        void RemoveSource(Source* source);
};

// Helper method for normal distribution generation
float RandomNormal(float mean, float dev);

// Closing preprocessor statement
#endif