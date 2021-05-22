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
void LoadState(const char* jsonPath, SimState* state, SimParams* params, SimSource* source)
{

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);

    // Update params with data
    params->lengthScale          = j["lengthScale"];
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

    //  Close filestream
    ifs.close();
}

// Load state into existing objects
void LoadState(const char* jsonPath, SimState* state, SimSource* source)
{

    // Open file
    std::ifstream ifs(jsonPath);
    json j = json::parse(ifs);

    SimParams* params = &(state->params);

    // Update params with data
    params->lengthScale          = j["lengthScale"];
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

    //  Close filestream
    ifs.close();
}