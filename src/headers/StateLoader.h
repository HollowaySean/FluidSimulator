/* Header file for JSON state loader */

// Preprocessor statement
#ifndef STATELOADER_H
#define STATELOADER_H

// Include files
#include <nlohmann/json.hpp>
#include <string>
#include "SimState.h"
#include "SimSource.h"
#include "Window.h"

// Global variables
extern std::string projectPath;

//// Functions ////

// Load state into existing objects
void LoadState(const char* jsonFilename, SimState* state, SimSource* source);

// Load state into existing objects (including params)
void LoadState(const char* jsonFilename, SimState* state, SimParams* params, SimSource* source);

// Load only parameters
void LoadParameters(const char* jsonFilename, SimParams* params);

// Load only sources
void LoadSources(const char* jsonFilename, SimSource* source);

// Load parameters
void LoadParams(nlohmann::json json, SimParams* params);

// Load sources
void LoadSources(nlohmann::json json, SimSource* source);

// Load window
void LoadWindow(nlohmann::json json, WindowProps* props);

// Load window from filename
void LoadWindow(const char* jsonFilename, WindowProps* props);

// Convert string to enum for shape
SimSource::Shape StringToShape(std::string shapeName);

// Convert string to enum for type
SimSource::Type StringToType(std::string typeName);

// Preprocessor end statement
#endif
