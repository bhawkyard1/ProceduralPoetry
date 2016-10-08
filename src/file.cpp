#include <iostream>

#include "common.hpp"
#include "file.hpp"
#include "util.hpp"

std::vector<std::string> getSources()
{
    std::vector<std::string> ret;

    std::ifstream src( g_RESOURCE_LOC + "sources.txt");

    std::string cur;

    while(getlineSafe(src, cur))
    {
        if(cur != "")
            ret.push_back(cur);
    }

    src.close();

    return ret;
}

std::vector<std::string> loadMarkovSource(const std::string _path)
{
    std::vector<std::string> ret;

    std::ifstream src( g_RESOURCE_LOC + "poems/" + _path );

    std::string cur;

    while(getlineSafe(src, cur))
    {
        //std::cout << "  " << cur << '\n';
        std::vector<std::string> splitVec =  split(cur, " ,.:;-?!()");
        splitVec.push_back("\n");
        ret.insert( ret.end(), splitVec.begin(), splitVec.end() );
    }

    src.close();

    return ret;
}
