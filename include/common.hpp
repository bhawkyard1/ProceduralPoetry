#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <random>

extern std::string g_RESOURCE_LOC;
extern std::default_random_engine g_RANDOM_TWISTER;

extern float g_TIME;

extern float g_PARAM_SAMPLE_TIMESTEP;
extern int g_PARAM_OCTAVE_MIN_CLIP;
extern int g_PARAM_OCTAVE_MAX_CLIP;
extern int g_PARAM_SAMPLE_WIDTH;
extern int g_PARAM_PEAK_WIDTH;
extern int g_PARAM_AVERAGED_WIDTH;
extern float g_PARAM_TRIGGER_THRESHOLD;

extern float g_PARAM_NOTE_INTENSITY_ACCUMULATION;

extern int g_PARAM_NOTESET_SIMILARITY_TOLERANCE_BUILD;
extern int g_PARAM_NOTESET_SIMILARITY_TOLERANCE_RUNTIME;
extern int g_PARAM_NOTESET_SIMILARITY_TOLERANCE;

#endif
