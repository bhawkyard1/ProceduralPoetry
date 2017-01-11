#include "common.hpp"
#include "file.hpp"
#include "printer.hpp"
#include "util.hpp"

std::string g_RESOURCE_LOC = "./resources/";
std::default_random_engine g_RANDOM_TWISTER ((std::random_device())());
std::string g_DEFAULT_FILE_PATH;

float g_TIME = 0;

int g_PARAM_NOTESET_SIMILARITY_TOLERANCE = 0/*g_PARAM_NOTESET_SIMILARITY_TOLERANCE_BUILD*/;

std::unordered_map<std::string, gvar> g_GLOBALS;

void loadConfig()
{
	printer pr;
	std::vector<std::string> lines = getLinesFromFile("config.txt");
	for(auto &line : lines)
	{
		if(line[0] == '#')
			continue;
		std::vector<std::string> spl = split(line, ' ');
		if(spl.size() == 0)
			continue;
		g_GLOBALS.insert( std::make_pair(spl[0], togvar(spl[1])) );
		pr.message( "Config value loaded " + spl[0] + " : " + std::to_string(g_GLOBALS[spl[0]].f) + " / " + std::to_string(g_GLOBALS[spl[0]].i) + '\n', YELLOW );
	}
}

gvar togvar(const std::string &_arg)
{
	gvar v;
	v.i = std::stoi(_arg);
	v.f = std::stof(_arg);
	return v;
}

long gint(const std::string &_var)
{
	return g_GLOBALS.at( _var ).i;
}

float gflt(const std::string &_var)
{
	return g_GLOBALS.at( _var ).f;
}
