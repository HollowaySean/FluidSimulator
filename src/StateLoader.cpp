/* Definition file for JSON state loader functions */

// Preprocessor statements
#include "headers/StateLoader.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

//// Functions ////

// Load state into existing objects (including params)
void LoadState(const char* jsonFilename, SimState* state, SimParams* params, SimSource* source)
{

    // Append JSON filename to correct path
    std::string jsonPath = projectPath + "/src/json/" + jsonFilename + ".json";

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);

    // Update params with data
    params->lengthScale          = j["lengthScale"];
    params->timeScale            = j["timeScale"];
    params->visc                 = j["visc"];
    params->diff                 = j["diff"];
    params->grav                 = j["grav"];
    params->airDens              = j["airDensity"];
    params->massRatio            = j["massRatio"];
    params->airTemp              = j["airTemp"];
    params->diffTemp             = j["diffTemp"];
    params->densDecay            = j["densDecay"];
    params->tempFactor           = j["tempFactor"];
    params->tempDecay            = j["tempDecay"];
    params->closedBoundaries     = j["closedBoundaries"];
    params->advancedCoefficients = j["advancedCoefficients"];
    params->gravityOn            = j["gravityOn"];
    params->temperatureOn        = j["temperatureOn"];
    params->solverSteps          = j["solverSteps"];

    // Set SimState parameters
    state->params = *params;

    // Load sources from file
    nlohmann::json sourceList = j["sources"];
    
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

    //  Close filestream
    ifs.close();
}

// Load state into existing objects
void LoadState(const char* jsonFilename, SimState* state, SimSource* source)
{

    // Append JSON filename to correct path
    std::string jsonPath = projectPath + "/src/json/" + jsonFilename + ".json";

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);

    SimParams* params = &(state->params);

    // Update params with data
    params->lengthScale          = j["lengthScale"];
    params->timeScale            = j["timeScale"];
    params->visc                 = j["visc"];
    params->diff                 = j["diff"];
    params->grav                 = j["grav"];
    params->airDens              = j["airDensity"];
    params->massRatio            = j["massRatio"];
    params->airTemp              = j["airTemp"];
    params->diffTemp             = j["diffTemp"];
    params->densDecay            = j["densDecay"];
    params->tempFactor           = j["tempFactor"];
    params->tempDecay            = j["tempDecay"];
    params->closedBoundaries     = j["closedBoundaries"];
    params->advancedCoefficients = j["advancedCoefficients"];
    params->gravityOn            = j["gravityOn"];
    params->temperatureOn        = j["temperatureOn"];
    params->solverSteps          = j["solverSteps"];

    // Set SimState parameters
    state->params = *params;

    // Load sources from file
    nlohmann::json sourceList = j["sources"];
    
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