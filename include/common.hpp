#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <random>

extern std::string g_RESOURCE_LOC;
extern std::default_random_engine g_RANDOM_TWISTER;

extern float g_TIME;

extern float g_PARAM_SAMPLE_TIMESTEP;
extern bool g_PARAM_USE_OCTAVES;
extern int g_PARAM_OCTAVE_MIN_CLIP;
extern int g_PARAM_OCTAVE_MAX_CLIP;
extern float g_PARAM_ACTIVATE_THRESHOLD_MUL;
extern int g_PARAM_SAMPLE_WIDTH;
extern int g_PARAM_PEAK_WIDTH;
extern int g_PARAM_AVERAGED_WIDTH;

#endif
