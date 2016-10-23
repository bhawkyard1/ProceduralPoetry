#ifndef UTIL_HPP
#define UTIL_HPP

#include <limits>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <random>

#include <ngl/Vec2.h>

#include "common.hpp"

//----------------------------------------------------------------------------------------------------------------------
/// \brief Max and infinite values for floats. Useful when you want to loop through a bunch of values to find the highest/
/// lowest.
//----------------------------------------------------------------------------------------------------------------------
#define F_MAX std::numeric_limits<float>::max()
#define F_INF std::numeric_limits<float>::infinity()

std::istream& getlineSafe(std::istream& is, std::string& t);

ngl::Vec2 getMousePos();

int randInt(int _low, int _high);
float randFlt(float _low, float _high);

int levenshtein(const std::string &_a, const std::string &_b);

std::vector<std::string> split(std::string _str, char _delim);
std::vector<std::string> split(std::string _str, std::string _delims);

void errorExit(const std::string &_msg);

float rad(const float _deg);

#endif
