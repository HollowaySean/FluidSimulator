/* Definition file for JSON state loader functions */

// Preprocessor statements
#include "headers/StateLoader.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

//// Functions ////

// Load state into existing objects (including params and props)
void LoadState(const char* jsonFilename, SimState* state, SimParams* params, SimSource* source)
{

    // Append JSON filename to correct path
    std::string jsonPath = projectPath + "/src/json/" + jsonFilename + ".json";

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);

    // Load simulation parameters
    LoadParams(j, params);

    // Set SimState parameters
    state->params = *params;

    // Load sources
    LoadSources(j, source);

    //  Close filestream
    ifs.close();
}

// Load state into existing objects, using params from state
void LoadState(const char* jsonFilename, SimState* state, SimSource* source)
{
    // Get params from state
    SimParams* params = &(state->params);

    // Run full method
    LoadState(jsonFilename, state, params, source);
}

// Load parameters
void LoadParams(nlohmann::json json, SimParams* params)
{
    // Update params with data
    params->lengthScale          = json["params"]["lengthScale"];
    params->timeScale            = json["params"]["timeScale"];
    params->visc                 = json["params"]["visc"];
    params->diff                 = json["params"]["diff"];
    params->grav                 = json["params"]["grav"];
    params->airDens              = json["params"]["airDensity"];
    params->massRatio            = json["params"]["massRatio"];
    params->airTemp              = json["params"]["airTemp"];
    params->diffTemp             = json["params"]["diffTemp"];
    params->densDecay            = json["params"]["densDecay"];
    params->tempFactor           = json["params"]["tempFactor"];
    params->tempDecay            = json["params"]["tempDecay"];
    params->closedBoundaries     = json["params"]["closedBoundaries"];
    params->advancedCoefficients = json["params"]["advancedCoefficients"];
    params->gravityOn            = json["params"]["gravityOn"];
    params->temperatureOn        = json["params"]["temperatureOn"];
    params->solverSteps          = json["params"]["solverSteps"];
}

// Load sources
void LoadSources(nlohmann::json json, SimSource* source)
{
    // Load sources from file
    nlohmann::json sourceList = json["sources"];
    
    // Loop through sources
    for(int i = 0; i < sourceList.size(); i++){

        // Switch by source type
        switch(StringToType(sourceList[i]["type"])){
            case SimSource::gas:
                source->CreateGasSource(StringToShape(sourceList[i]["shape"]), 
                    sourceList[i]["flowRate"], sourceList[i]["sourceTemp"],
                    sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                    sourceList[i]["radius"]);
                break;
            case SimSource::wind:
                source->CreateWindSource(
                    sourceList[i]["angle"], sourceList[i]["speed"],
                    sourceList[i]["xCenter"], sourceList[i]["yCenter"]);
                break;
            case SimSource::windBoundary:
                source->CreateWindBoundary(
                    sourceList[i]["speed"]);
                break;
            case SimSource::heat:
                source->CreateHeatSource(StringToShape(sourceList[i]["shape"]), 
                    sourceList[i]["sourceTemp"],
                    sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                    sourceList[i]["radius"]);
                break;
            case SimSource::energy:
                source->CreateEnergySource(StringToShape(sourceList[i]["shape"]), 
                    sourceList[i]["flux"], sourceList[i]["referenceTemp"], sourceList[i]["referenceDensity"],
                    sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                    sourceList[i]["radius"]);
                break;
        }
    }

    // Implement sources
    source->UpdateSources();
}

// Load window settings
void LoadWindow(nlohmann::json json, WindowProps* props)
{
    props->resolution   = json["windowProps"]["resolution"];
    props->winWidth     = json["windowProps"]["winWidth"];
    props->controlWidth = json["windowProps"]["controlWidth"];
    props->maxFrameRate = json["windowProps"]["maxFrameRate"];
}

// Load window directly
void LoadWindow(const char* jsonFilename, WindowProps* props)
{

    // Append JSON filename to correct path
    std::string jsonPath = projectPath + "/src/json/" + jsonFilename + ".json";

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);
    
    // Load properties into object
    LoadWindow(j, props);

    //  Close filestream
    ifs.close();

}

// Convert string to enum for shape
SimSource::Shape StringToShape(std::string shapeName)
{
    if(shapeName.compare("square") == 0)    { return SimSource::square; }
    if(shapeName.compare("circle") == 0)    { return SimSource::circle; }
    if(shapeName.compare("diamond") == 0)   { return SimSource::diamond; }
    if(shapeName.compare("point") == 0)     { return SimSource::point; }

    // Default for empty case
    return SimSource::point;
}

// Convert string to enum for type
SimSource::Type StringToType(std::string typeName)
{
    if(typeName.compare("gas") == 0)            { return SimSource::gas; }
    if(typeName.compare("wind") == 0)           { return SimSource::wind; }
    if(typeName.compare("windBoundary") == 0)   { return SimSource::windBoundary; }
    if(typeName.compare("heat") == 0)           { return SimSource::heat; }
    if(typeName.compare("energy") == 0)         { return SimSource::energy; }

    // Default for empty case
    return SimSource::gas;
}