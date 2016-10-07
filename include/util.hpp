#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>

#include <limits>


//----------------------------------------------------------------------------------------------------------------------
/// \brief Max and infinite values for floats. Useful when you want to loop through a bunch of values to find the highest/
/// lowest.
//----------------------------------------------------------------------------------------------------------------------
#define F_MAX std::numeric_limits<float>::max()
#define F_INF std::numeric_limits<float>::infinity()

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

#endif
