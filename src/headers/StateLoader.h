/* Header file for JSON state loader */

// Preprocessor statement
#ifndef STATELOADER_H
#define STATELOADER_H

// Include files
#include <nlohmann/json.hpp>
#include <string.h>
#include "SimState.h"
#include "SimSource.h"

//// Functions ////

// Load state into existing objects
void LoadState(const char* jsonPath, SimState* state, SimSource* source);

// Load state into existing objects (including params)
void LoadState(const char* jsonPath, SimState* state, SimParams* params, SimSource* source);

// Preprocessor end statement
#endif
