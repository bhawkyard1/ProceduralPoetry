#ifndef UTIL_HPP
#define UTIL_HPP

#include <limits>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <random>

#include <ngl/Vec2.h>
#include <ngl/Vec3.h>

#include <SDL2/SDL.h>

#include "common.hpp"

//----------------------------------------------------------------------------------------------------------------------
/// \brief Max and infinite values for floats. Useful when you want to loop through a bunch of values to find the highest/
/// lowest.
//----------------------------------------------------------------------------------------------------------------------
#define F_MAX std::numeric_limits<float>::max()
#define F_INF std::numeric_limits<float>::infinity()

template<typename tt>
tt clamp(tt _v, tt _m, tt _M)
{
    if(_v < _m) return _m;
    else if(_v > _M) return _M;
    return _v;
}

std::istream& getlineSafe(std::istream& is, std::string& t);

ngl::Vec2 getMousePos();
ngl::Vec2 getMousePosWindowCorrective( SDL_Window * _window);

int randInt(int _low, int _high);
float randFlt(float _low, float _high);

int levenshtein(const std::string &_a, const std::string &_b);

std::pair<std::string, std::string> splitFirst(std::string _str, char _delim);
std::vector<std::string> split(std::string _str, char _delim);
std::vector<std::string> split(std::string _str, std::string _delims);

void errorExit(const std::string &_msg);

float rad(const float _deg);

std::string toString(const std::vector<std::string> &_vec );

template<class T>
void fifoQueue(std::vector<T> *_vec, const T &_entry, size_t _order)
{
    _vec->push_back(_entry);
    if(_vec->size() > _order)
        _vec->erase( _vec->begin() );
}

std::pair<ngl::Vec3, ngl::Vec3> lim(const std::vector<ngl::Vec3> &_vecs);

template<class T>
T sqr(T _arg)
{
    return _arg * _arg;
}

int mod(int _x, int _m);

template<class T>
void averageVector(const std::vector<T> &_in, std::vector<T> &_out, size_t _width)
{
	_out.reserve( _in.size() / _width );
	for(size_t i = 0; i < _in.size(); i += _width)
	{
		T insert = 0;
		for(size_t j = i; j < i + _width; ++j)
			insert += _in[j];
		insert /= _width;
		_out.push_back( insert );
	}
}

#endif
