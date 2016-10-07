#include "common.hpp"
#include "file.hpp"
#include "util.hpp"

std::vector<std::string> loadSource(const std::string _path)
{
    std::vector<std::string> ret;

    std::ifstream src( g_RESOURCE_LOC + _path);

    std::string cur;

    while(getlineSafe(src, cur))
    {
        std::vector<std::string> splitVec =  split(cur, " ,.:;-?!");
        ret.insert( ret.end(), splitVec.begin(), splitVec.end() );
    }

    src.close();

    return ret;
}
