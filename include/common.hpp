//----------------------------------------------------------------------------------------------------------------------
/// \file common.hpp
/// \brief This file contains the global variables used throughout the program.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
//----------------------------------------------------------------------------------------------------------------------

#ifndef COMMON_HPP
#define COMMON_HPP

#include <unordered_map>
#include <string>
#include <random>

extern std::string g_RESOURCE_LOC;
extern std::default_random_engine g_RANDOM_TWISTER;
extern std::string g_DEFAULT_FILE_PATH;
extern std::string g_SOURCE;

extern float g_TIME;
extern float g_TIME_SCALE;

extern int g_PARAM_NOTESET_SIMILARITY_TOLERANCE;

struct gvar
{
		long i;
    float f;
};

extern std::unordered_map<std::string, gvar> g_GLOBALS;

void loadConfig();
gvar togvar(const std::string &_arg);

long gint(const std::string &_var);
float gflt(const std::string &_var);

#endif
