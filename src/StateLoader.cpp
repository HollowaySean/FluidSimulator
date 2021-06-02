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

// Load only parameters
void LoadParameters(const char* jsonFilename, SimParams* params)
{
    // Append JSON filename to correct path
    std::string jsonPath = projectPath + "/src/json/" + jsonFilename + ".json";

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);

    // Load simulation parameters
    LoadParams(j, params);

    //  Close filestream
    ifs.close();
}

// Load only sources
void LoadSources(const char* jsonFilename, SimSource* source)
{
    // Append JSON filename to correct path
    std::string jsonPath = projectPath + "/src/json/" + jsonFilename + ".json";

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);

    // Load simulation parameters
    LoadSources(j, source);

    //  Close filestream
    ifs.close();

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
                if(sourceList[i]["isDynamic"]){
                    source->CreateGasSourceDynamic(StringToShape(sourceList[i]["shape"]), 
                        sourceList[i]["flowRate"], sourceList[i]["sourceTemp"],
                        sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                        sourceList[i]["radius"], sourceList[i]["flowVar"], sourceList[i]["tempVar"]);
                }else{
                    source->CreateGasSource(StringToShape(sourceList[i]["shape"]), 
                        sourceList[i]["flowRate"], sourceList[i]["sourceTemp"],
                        sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                        sourceList[i]["radius"]);
                }
                break;
            case SimSource::wind:
                if(sourceList[i]["isDynamic"]){
                    source->CreateWindSourceDynamic(
                        sourceList[i]["angle"], sourceList[i]["speed"],
                        sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                        sourceList[i]["speedVar"], sourceList[i]["angleVar"]);
                }else{
                    source->CreateWindSource(
                        sourceList[i]["angle"], sourceList[i]["speed"],
                        sourceList[i]["xCenter"], sourceList[i]["yCenter"]);
                }
                break;
            case SimSource::windBoundary:
                if(sourceList[i]["isDynamic"]){
                    source->CreateWindBoundaryDynamic(
                        sourceList[i]["speed"], sourceList[i]["speedVar"]);
                }else{
                    source->CreateWindBoundary(
                        sourceList[i]["speed"]);
                }
                break;
            case SimSource::heat:
                if(sourceList[i]["isDynamic"]){
                    source->CreateHeatSourceDynamic(StringToShape(sourceList[i]["shape"]), 
                        sourceList[i]["sourceTemp"],
                        sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                        sourceList[i]["radius"], sourceList[i]["tempVar"]);
                }else{
                    source->CreateHeatSource(StringToShape(sourceList[i]["shape"]), 
                        sourceList[i]["sourceTemp"],
                        sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                        sourceList[i]["radius"]);
                }
                break;
            case SimSource::energy:
                if(sourceList[i]["isDynamic"]){
                    source->CreateEnergySourceDynamic(StringToShape(sourceList[i]["shape"]), 
                        sourceList[i]["flux"], sourceList[i]["referenceTemp"], sourceList[i]["referenceDensity"],
                        sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                        sourceList[i]["radius"], sourceList[i]["fluxVar"]);
                }else{
                    source->CreateEnergySource(StringToShape(sourceList[i]["shape"]), 
                        sourceList[i]["flux"], sourceList[i]["referenceTemp"], sourceList[i]["referenceDensity"],
                        sourceList[i]["xCenter"], sourceList[i]["yCenter"],
                        sourceList[i]["radius"]);
                }
                break;
        }
    }

    // Implement sources
    source->UpdateSourcesDynamic();
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

// Load window settings
void LoadRecord(nlohmann::json json, WindowProps* props)
{
    props->resolution   = json["recordProps"]["resolution"];
    props->winWidth     = json["recordProps"]["winWidth"];
    props->fps          = json["recordProps"]["fps"];
    props->numFrames    = json["recordProps"]["numFrames"];
}

// Load window directly
void LoadRecord(const char* jsonFilename, WindowProps* props)
{

    // Append JSON filename to correct path
    std::string jsonPath = projectPath + "/src/json/" + jsonFilename + ".json";

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);
    
    // Load properties into object
    LoadRecord(j, props);

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