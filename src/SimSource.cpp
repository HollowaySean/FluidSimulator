
// Include statements
#include "headers/SimSource.h"
#include <cmath>
#include <iostream>
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

// Update sources with dynamic processes
void SimSource::UpdateSourcesDynamic()
{
    // Zero out sources first
    simState -> ResetSources();

    // Loop through list of sources
    for (const Source* source : sources){

        if(source -> isDynamic){

            // Loop through source indices
            for(const int & index : source -> indices){

                float ang, spd;

                switch(source -> type){
                    case gas:
                        dens[index] += RandomNormal(source -> dens, source -> dVar);
                        temp[index] = max(temp[index], RandomNormal(source -> temp, source -> tVar));
                        break;
                    case wind:
                        ang = RandomNormal(source -> aMean, source -> aVar);
                        spd = RandomNormal(source -> wMean, source -> wVar);
                        xVel[index] += spd * cos(ang * 3.14159265 / 180.0);
                        yVel[index] += spd * sin(ang * 3.14159265 / 180.0);
                        break;
                    case heat:
                        temp[index] = max(temp[index], RandomNormal(source -> temp, source -> tVar));
                        break;
                    case energy:
                        temp[index] = max(temp[index], RandomNormal(source -> temp, source -> tVar));
                        break;
                    case windBoundary:
                        xVel[index] += RandomNormal(source -> wMean, source -> wVar);
                        break;
                }
            }

        }else{

            // Loop through source indices
            for(const int & index : source -> indices){

                xVel[index] += source -> xVel;
                yVel[index] += source -> yVel;
                dens[index] += source -> dens;
                temp[index] = max(temp[index], source -> temp);
            }

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

    // Return single point if size is too small to cover any integral points
    if(indices.size() == 0){
        indices.push_back(indN(int(round(xCInd)), int(round(yCInd)), N));
    }
}



// Create gas source and add to source list
void SimSource::CreateGasSource(Shape shape, float flowRate, float sourceTemp, float xCenter, float yCenter, float radius)
{
    GasSource* newGasSource = new GasSource(simState->GetN(), simState->params.lengthScale, shape, flowRate, sourceTemp, xCenter, yCenter, radius);
    Source* newSource = newGasSource;
    sources.push_back(newSource);
}

// Create dynamic gas source and add to source list
void SimSource::CreateGasSourceDynamic(Shape shape, float flowRate, float sourceTemp, float xCenter, float yCenter, float radius, float flowVar, float tempVar)
{
    GasSource* newGasSource = new GasSource(simState->GetN(), simState->params.lengthScale, shape, flowRate, sourceTemp, xCenter, yCenter, radius);
    newGasSource -> isDynamic = true;
    newGasSource -> dVar = flowVar;
    newGasSource -> tVar = tempVar;
    Source* newSource = newGasSource;
    sources.push_back(newSource);
}

// Gas source constructor
SimSource::GasSource::GasSource(int N, float lengthScale, Shape shape, float flowRate, float sourceTemp, float xCenter, float yCenter, float radius)
{
    // Set source indices
    this -> xCenter = xCenter;
    this -> yCenter = yCenter;
    this -> radius = radius;
    this -> shape = shape;
    SetIndices(N, shape, xCenter, yCenter, radius);

    // Calculate sources
    this -> type = gas;
    this -> dens = flowRate / indices.size();
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

// Create dynamic wind source and add to source list
void SimSource::CreateWindSourceDynamic(float angle, float speed, float xCenter, float yCenter, float speedVar, float angleVar)
{
    WindSource* newWindSource = new WindSource(simState->GetN(), simState->params.lengthScale, angle, speed, xCenter, yCenter);
    newWindSource -> isDynamic = true;
    newWindSource -> wMean = speed;
    newWindSource -> aMean = angle;
    newWindSource -> wVar = speedVar;
    newWindSource -> aVar = angleVar;
    Source* newSource = newWindSource;
    sources.push_back(newSource);
}

// Wind source constructor
SimSource::WindSource::WindSource(int N, float lengthScale, float angle, float speed, float xCenter, float yCenter)
{
    // Calculate sources
    this -> type = wind;
    this -> dens = 0.0;
    this -> temp = 0.0;
    this -> xVel = speed * cos(angle * 3.1415926 / 180.0);
    this -> yVel = speed * sin(angle * 3.1415926 / 180.0);

    // Set source indices
    this -> xCenter = xCenter;
    this -> yCenter = yCenter;
    this -> radius = 0.0;
    this -> shape = point;
    SetIndices(N, point, xCenter, yCenter, 0.0);
}

// Create heat source and add to source list
void SimSource::CreateHeatSource(Shape shape, float sourceTemp, float xCenter, float yCenter, float radius)
{
    HeatSource* newHeatSource = new HeatSource(simState->GetN(), simState->params.lengthScale, shape, sourceTemp, xCenter, yCenter, radius);
    Source* newSource = newHeatSource;
    sources.push_back(newSource);
}

// Create heat source and add to source list
void SimSource::CreateHeatSourceDynamic(Shape shape, float sourceTemp, float xCenter, float yCenter, float radius, float tempVar)
{
    HeatSource* newHeatSource = new HeatSource(simState->GetN(), simState->params.lengthScale, shape, sourceTemp, xCenter, yCenter, radius);
    newHeatSource -> isDynamic = true;
    newHeatSource -> tVar = tempVar;
    Source* newSource = newHeatSource;
    sources.push_back(newSource);
}

// Heat source constructor
SimSource::HeatSource::HeatSource(int N, float lengthScale, Shape shape, float sourceTemp, float xCenter, float yCenter, float radius)
{
    // Calculate sources
    this -> type = heat;
    this -> dens = 0.0;
    this -> temp = sourceTemp;
    this -> xVel = 0.0;
    this -> yVel = 0.0;

    // Set source indices
    this -> xCenter = xCenter;
    this -> yCenter = yCenter;
    this -> radius = radius;
    this -> shape = shape;
    SetIndices(N, shape, xCenter, yCenter, radius);
}

// Create gas source and add to source list
void SimSource::CreateEnergySource(Shape shape, float flux, float referenceTemp, float referenceDensity, float xCenter, float yCenter, float radius)
{
    EnergySource* newEnergySource = new EnergySource(simState->GetN(), simState->params.lengthScale, shape, flux, referenceTemp, referenceDensity, xCenter, yCenter, radius);
    Source* newSource = newEnergySource;
    sources.push_back(newSource);
}

// Create gas source and add to source list
void SimSource::CreateEnergySourceDynamic(Shape shape, float flux, float referenceTemp, float referenceDensity, float xCenter, float yCenter, float radius, float fluxVar)
{
    EnergySource* newEnergySource = new EnergySource(simState->GetN(), simState->params.lengthScale, shape, flux, referenceTemp, referenceDensity, xCenter, yCenter, radius);
    newEnergySource -> isDynamic = true;
    newEnergySource -> tVar = fluxVar;
    Source* newSource = newEnergySource;
    sources.push_back(newSource);
}

// Gas source constructor
SimSource::EnergySource::EnergySource(int N, float lengthScale, Shape shape, float flux, float referenceTemp,  float referenceDensity, float xCenter, float yCenter, float radius)
{
    // Set source indices
    this -> xCenter = xCenter;
    this -> yCenter = yCenter;
    this -> radius = radius;
    this -> shape = shape;
    SetIndices(N, shape, xCenter, yCenter, radius);

    // Calculate sources
    // NOTE: 12.5 adjusts for simple linear heat transfer
    this -> type = energy;
    this -> dens = 0.0;
    this -> temp = referenceTemp + (flux / (12.5 * referenceDensity * indices.size()));
    this -> xVel = 0.0;
    this -> yVel = 0.0;
}

// Create wind across left and right boundaries
void SimSource::CreateWindBoundary(float speed)
{
    // Remove any other wind boundaries
    for(Source* source : sources){
        if(source -> type == windBoundary){
            RemoveSource(source);
            return;
        }
    }

    WindBoundary* newWindBoundary = new WindBoundary(simState->GetN(), speed);
    Source* newSource = newWindBoundary;
    sources.push_back(newSource);
}

// Create wind across left and right boundaries
void SimSource::CreateWindBoundaryDynamic(float speed, float speedVar)
{
    // Remove any other wind boundaries
    for(Source* source : sources){
        if(source -> type == windBoundary){
            RemoveSource(source);
            return;
        }
    }

    WindBoundary* newWindBoundary = new WindBoundary(simState->GetN(), speed);
    newWindBoundary -> isDynamic = true;
    newWindBoundary -> wVar = speedVar;
    newWindBoundary -> wMean = speed;
    Source* newSource = newWindBoundary;
    sources.push_back(newSource);
}

// Wind boundary constructor
SimSource::WindBoundary::WindBoundary(int N, float speed)
{
    // Set sources
    this -> type = windBoundary;
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
        sources.remove(sourceToRemove);
        delete sourceToRemove;

    // Propogate change to simulation
    simState->ResetSources();
    UpdateSources();
}

// Find source that overlaps with point and remove it
void SimSource::RemoveSourceAtPoint(float x, float y, float dist)
{
    // Loop through sources
    for(Source* source : sources){

        // Check if point is inside source
        if(typeid(source).name() == "WindBoundary"){
            continue;
        }else{

            float rad = source->radius + dist;
            float xDist = abs(x - source->xCenter);
            float yDist = abs(y - source->yCenter);

            switch(source->shape){
                case circle:
                    if(xDist * xDist + yDist * yDist < rad * rad){
                        RemoveSource(source);
                        return;
                    }
                    break;

                case square:
                    if((xDist < rad) 
                    && (yDist < rad)){
                        RemoveSource(source);
                        return;
                    }
                    break;

                case diamond:
                    if(xDist + yDist < rad){
                        RemoveSource(source);
                        return;
                    }
                    break;

                case point:
                    if(xDist * xDist + yDist * yDist < rad * rad){
                        RemoveSource(source);
                        return;
                    }

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

// Non-class functions //

// Generate normal distributed random variable
float RandomNormal(float mean, float dev)
{
    // Don't bother using generator if no deviation
    if(dev == 0.0){
        return mean;
    }

    // Initialize a generator and return a random value
    static default_random_engine generator(time(0));
    normal_distribution<float> distribution(mean, dev);
    return distribution(generator);
}