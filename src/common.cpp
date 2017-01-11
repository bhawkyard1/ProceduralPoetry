#include "common.hpp"
#include "file.hpp"
#include "util.hpp"

std::string g_RESOURCE_LOC = "./resources/";
std::default_random_engine g_RANDOM_TWISTER ((std::random_device())());
std::string g_DEFAULT_FILE_PATH;

float g_TIME = 0;

/*float g_PARAM_SAMPLE_TIMESTEP = 1.0f / 60.0f;
int g_PARAM_OCTAVE_MIN_CLIP = 0;
int g_PARAM_OCTAVE_MAX_CLIP = 6;
int g_PARAM_SAMPLE_WIDTH = 16384;

float g_PARAM_NOTE_INTENSITY_ACCUMULATION = 0.5f;

float g_PARAM_TRIGGER_THRESHOLD = 2.0f;
int g_PARAM_NOTESET_SIMILARITY_TOLERANCE_BUILD = 0;
int g_PARAM_NOTESET_SIMILARITY_TOLERANCE_RUNTIME = 0;*/

int g_PARAM_NOTESET_SIMILARITY_TOLERANCE = gint("noteset_similarity_tolerance_build")/*g_PARAM_NOTESET_SIMILARITY_TOLERANCE_BUILD*/;

std::unordered_map<std::string, gvar> g_GLOBALS;

void loadConfig()
{
    std::vector<std::string> lines = getLinesFromFile("config.txt");
    for(auto &line : lines)
    {
        if(line[0] == '#')
            continue;
        std::vector<std::string> spl = split(line, ' ');
        if(spl.size() == 0)
            continue;
        g_GLOBALS[spl[0]] = togvar(spl[1]);
    }
}

gvar togvar(const std::string &_arg)
{
    gvar v;
    /**v.i = std::stoi(_arg);
    v.f = std::stof(_arg);*/
    return v;
}

int gint(const std::string &_var)
{
    return g_GLOBALS.at(_var).i;
}

float gflt(const std::string &_var)
{
    return g_GLOBALS.at(_var).f;
}
