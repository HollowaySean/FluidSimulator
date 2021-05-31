
// Include statements
#include "headers/SimSource.h"
#include <cmath>
using namespace std;

// Macros
#define indN(i,j,N) ((i) + ((N) + 2)*(j))

/// SIMSOURCES METHODS ///

// Constructor, taking SimState reference as input
SimSource::SimSource(SimState* simState)
{
    // Save pointer to SimState
    this -> simState = simState;

    // Retrieve pointers to source arrays
    xVel = simState -> fields.xVel_source;
    yVel = simState -> fields.yVel_source;
    dens = simState -> fields.dens_source;
    temp = simState -> fields.temp_source;
}

// Update sources in SimState object
void SimSource::UpdateSources()
{
    // Zero out sources first
    simState -> ResetSources();

    // Loop through list of sources
    for (const Source* source : sources){

        // Loop through source indices
        for(const int & index : source -> indices){

            xVel[index] += source -> xVel;
            yVel[index] += source -> yVel;
            dens[index] += source -> dens;
            temp[index] = max(temp[index], source -> temp);
        }
    }
}

// Calculate indices covered by shape
void SimSource::Source::SetIndices(int N, Shape shape, float xCenter, float yCenter, float radius)
{
    float xCInd = float(N + 2) * (xCenter + 1.0) / 2.0;
    float yCInd = float(N + 2) * (yCenter + 1.0) / 2.0;

    if(shape == point){
        indices.push_back(indN(int(round(xCInd)), int(round(yCInd)), N));
        return;
    }

    float rInd  = N * radius / 2.0;
    float xMinInd = max(floor(xCInd - rInd), 0.0f);
    float xMaxInd = min(ceil( xCInd + rInd), float(N+2));
    float yMinInd = max(floor(yCInd - rInd), 0.0f);
    float yMaxInd = min(ceil( yCInd + rInd), float(N+2));


    for(float x = xMinInd; x <= xMaxInd; x++){
        for(float y = yMinInd; y <= yMaxInd; y++){

            switch(shape){

                case square:
                    indices.push_back(indN(int(x), float(y), N));
                    break;

                case circle:
                    if((x - xCInd) * (x - xCInd) + (y - yCInd) * (y - yCInd) <= rInd * rInd){
                        indices.push_back(indN(int(x), float(y), N));
                    }
                    break;

                case diamond:
                    if(abs(x - xCInd) + abs(y - yCInd) <= rInd ){
                        indices.push_back(indN(int(x), float(y), N));
                    }
                    break;
            }
        }
    }
}



// Create gas source and add to source list
void SimSource::CreateGasSource(Shape shape, float flowRate, float sourceTemp, float xCenter, float yCenter, float radius)
{
    GasSource* newGasSource = new GasSource(simState->GetN(), simState->params.lengthScale, shape, flowRate, sourceTemp, xCenter, yCenter, radius);
    Source* newSource = newGasSource;
    sources.push_back(newSource);
}

// Gas source constructor
SimSource::GasSource::GasSource(int N, float lengthScale, Shape shape, float flowRate, float sourceTemp, float xCenter, float yCenter, float radius)
{
    // Calculate aperture size
    float sourceSize = radius * radius * lengthScale * lengthScale;
    switch(shape){
        case square:
            sourceSize *= 1.0;
            break;
        case circle:
            sourceSize *= 3.141526 / 4.0;
            break;
        case diamond:
            sourceSize *= 0.5;
    }

    // Set source indices
    this -> xCenter = xCenter;
    this -> yCenter = yCenter;
    SetIndices(N, shape, xCenter, yCenter, radius);

    // Calculate sources
    this -> dens = flowRate / (sourceSize * indices.size());
    this -> temp = sourceTemp;
    this -> xVel = 0.0;
    this -> yVel = 0.0;
}

// Create wind source and add to source list
void SimSource::CreateWindSource(float angle, float speed, float xCenter, float yCenter)
{
    WindSource* newWindSource = new WindSource(simState->GetN(), simState->params.lengthScale, angle, speed, xCenter, yCenter);
    Source* newSource = newWindSource;
    sources.push_back(newSource);
}

// Wind source constructor
SimSource::WindSource::WindSource(int N, float lengthScale, float angle, float speed, float xCenter, float yCenter)
{
    // Calculate sources
    this -> dens = 0.0;
    this -> temp = 0.0;
    this -> xVel = speed * cos(angle * 3.1415926 / 180.0);
    this -> yVel = speed * sin(angle * 3.1415926 / 180.0);

    // Set source indices
    this -> xCenter = xCenter;
    this -> yCenter = yCenter;
    SetIndices(N, point, xCenter, yCenter, 0.0);
}

// Create heat source and add to source list
void SimSource::CreateHeatSource(Shape shape, float sourceTemp, float xCenter, float yCenter, float radius)
{
    HeatSource* newHeatSource = new HeatSource(simState->GetN(), simState->params.lengthScale, shape, sourceTemp, xCenter, yCenter, radius);
    Source* newSource = newHeatSource;
    sources.push_back(newSource);
}

// Heat source constructor
SimSource::HeatSource::HeatSource(int N, float lengthScale, Shape shape, float sourceTemp, float xCenter, float yCenter, float radius)
{
    // Calculate sources
    this -> dens = 0.0;
    this -> temp = sourceTemp;
    this -> xVel = 0.0;
    this -> yVel = 0.0;

    // Set source indices
    this -> xCenter = xCenter;
    this -> yCenter = yCenter;
    SetIndices(N, shape, xCenter, yCenter, radius);
}

// Create gas source and add to source list
void SimSource::CreateEnergySource(Shape shape, float flux, float referenceTemp, float referenceDensity, float xCenter, float yCenter, float radius)
{
    EnergySource* newEnergySource = new EnergySource(simState->GetN(), simState->params.lengthScale, shape, flux, referenceTemp, referenceDensity, xCenter, yCenter, radius);
    Source* newSource = newEnergySource;
    sources.push_back(newSource);
}

// Gas source constructor
SimSource::EnergySource::EnergySource(int N, float lengthScale, Shape shape, float flux, float referenceTemp,  float referenceDensity, float xCenter, float yCenter, float radius)
{
    // Calculate aperture size
    float sourceSize = radius * radius * lengthScale * lengthScale;
    switch(shape){
        case square:
            sourceSize *= 1.0;
            break;
        case circle:
            sourceSize *= 3.141526 / 4.0;
            break;
        case diamond:
            sourceSize *= 0.5;
    }

    // Set source indices
    this -> xCenter = xCenter;
    this -> yCenter = yCenter;
    SetIndices(N, shape, xCenter, yCenter, radius);

    // Calculate sources
    this -> dens = 0.0;
    this -> temp = referenceTemp + (flux / (12.5 * referenceDensity * sourceSize * indices.size()));
    this -> xVel = 0.0;
    this -> yVel = 0.0;
}

// Create wind across left and right boundaries
void SimSource::CreateWindBoundary(float speed)
{
    WindBoundary* newWindBoundary = new WindBoundary(simState->GetN(), speed);
    Source* newSource = newWindBoundary;
    sources.push_back(newSource);
}

// Wind boundary constructor
SimSource::WindBoundary::WindBoundary(int N, float speed)
{
    // Set sources
    this -> dens = 0.0;
    this -> temp = 0.0;
    this -> xVel = speed;
    this -> yVel = 0.0;

    // Set source indices
    for(int i = 0; i < N+1; i++){

        // Set left and right boundaries
        indices.push_back(indN(1,i,N));
        indices.push_back(indN(N,i,N));
    }
}



// Remove source
void SimSource::RemoveSource(Source* sourceToRemove)
{
    // Remove from source list
    this->sources.remove(sourceToRemove);
    delete sourceToRemove;

    // Propogate change to simulation
    this->UpdateSources();
}

// Find source that overlaps with point and remove it
void SimSource::RemoveSourceAtPoint(float x, float y)
{
    // Loop through sources
    for(Source* source : sources){

        // Check if point is inside source
        if(typeid(source).name() == "WindBoundary"){
            continue;
        }else{

            float xDist = abs(x - source->xCenter);
            float yDist = abs(y - source->yCenter);

            switch(source->shape){
                case circle:
                    if(xDist * xDist + yDist * yDist < source->radius * source->radius){
                        RemoveSource(source);
                        return;
                    }
                    break;
                case square:

                    if((xDist < source->radius) 
                    && (yDist < source->radius)){
                        RemoveSource(source);
                        return;
                    }
                    break;

                case diamond:

                    if(xDist + yDist < source->radius){
                        RemoveSource(source);
                        return;
                    }
                    break;
            }
        }
    }
}

// Remove all sources
void SimSource::RemoveAllSources()
{
    // Pop all and reset state
    while(sources.size() > 0){
        delete sources.back();
        sources.pop_back();
    }
    simState->ResetSources();
    UpdateSources();
}

// Resize grid to N + 2 by N + 2
void SimSource::Reset()
{
    // Remove all sources
    RemoveAllSources();

    // Retrieve pointers to source arrays
    xVel = simState -> fields.xVel_source;
    yVel = simState -> fields.yVel_source;
    dens = simState -> fields.dens_source;
    temp = simState -> fields.temp_source;
}