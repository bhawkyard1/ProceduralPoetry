#include "common.hpp"

std::string g_RESOURCE_LOC = "./resources/";
std::default_random_engine g_RANDOM_TWISTER ((std::random_device())());

float g_TIME = 0;

float g_PARAM_SAMPLE_TIMESTEP = 1.0f / 60.0f;
bool g_PARAM_USE_OCTAVES = true;
int g_PARAM_OCTAVE_MIN_CLIP = 0;
int g_PARAM_OCTAVE_MAX_CLIP = 6;
int g_PARAM_SAMPLE_WIDTH = 4096;
/*float g_PARAM_ACTIVATE_THRESHOLD_MUL = 1.3f;
int g_PARAM_PEAK_WIDTH = 512;
int g_PARAM_AVERAGED_WIDTH = 4096;*/
float g_PARAM_TRIGGER_THRESHOLD = /*0.0315f*/1.2f /* (16384.0f / g_PARAM_SAMPLE_WIDTH)*/;
float g_PARAM_DENOISE_MUL = 1.1f;
