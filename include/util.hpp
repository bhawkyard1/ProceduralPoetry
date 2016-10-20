#ifndef UTIL_HPP
#define UTIL_HPP

#include <limits>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
/// \brief Max and infinite values for floats. Useful when you want to loop through a bunch of values to find the highest/
/// lowest.
//----------------------------------------------------------------------------------------------------------------------
#define F_MAX std::numeric_limits<float>::max()
#define F_INF std::numeric_limits<float>::infinity()

std::istream& getlineSafe(std::istream& is, std::string& t);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Returns a random number between two values.
/// \param _low lover limit
/// \param _high upper limit
//----------------------------------------------------------------------------------------------------------------------
template<class t>
t randNum(t _low, t _high)
{
    if(_low == _high) return _low;
    return static_cast <t> (rand()) / static_cast <t> (RAND_MAX/(_high-_low))+_low;
}

int levenshtein(const std::string &_a, const std::string &_b);

std::vector<std::string> split(std::string _str, char _delim);
std::vector<std::string> split(std::string _str, std::string _delims);

void errorExit(const std::string &_msg);

#endif
