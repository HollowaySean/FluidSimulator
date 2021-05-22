/* Definition file for JSON state loader functions */

// Preprocessor statements
#include "headers/StateLoader.h"
#include <stdio.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

//// Functions ////

// Load state into existing objects
void LoadState(const char* jsonPath, SimState* state, SimParams* params, SimSource* sources)
{

    // Open file
    FILE * pFile;
    pFile = fopen(jsonPath, "r");

    // Read JSON data
    json j = json::parse(pFile);

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
}