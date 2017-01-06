#include "common.hpp"

std::string g_RESOURCE_LOC = "./resources/";
std::default_random_engine g_RANDOM_TWISTER ((std::random_device())());

float g_TIME = 0;

float g_PARAM_SAMPLE_TIMESTEP = 1.0f / 30.0f;
bool g_PARAM_USE_OCTAVES = true;
int g_PARAM_OCTAVE_MIN_CLIP = 0;
int g_PARAM_OCTAVE_MAX_CLIP = 5;
float g_PARAM_ACTIVATE_THRESHOLD_MUL = 1.2f;
int g_PARAM_SAMPLE_WIDTH = 8192;
int g_PARAM_PEAK_WIDTH = 256;
int g_PARAM_AVERAGED_WIDTH = 4096;

//11936
