#include <algorithm>
#include <cmath>
#include <iostream>

#ifdef _WIN32
#include <ciso646>
#endif

#include "notes.hpp"
#include "util.hpp"

notetype operator+(const notetype &_lhs, const int &_rhs)
{
	return static_cast<notetype>(static_cast<int>(_lhs) + _rhs);
}

bool operator==(const note &_lhs, const note &_rhs)
{
	return _lhs.m_position == _rhs.m_position and _lhs.m_type == _rhs.m_type;
}

/*bool operator<(const note &_lhs, const note &_rhs)
{
	return _lhs.m_position < _rhs.m_position or static_cast<int>(_lhs.m_type) < static_cast<int>(_rhs.m_type);
}*/

bool operator>(const note &_lhs, const note &_rhs)
{
	return _lhs.m_position > _rhs.m_position or static_cast<int>(_lhs.m_type) > static_cast<int>(_rhs.m_type);
}

note operator+(const note &_lhs, const int _steps)
{
	note ret = _lhs;

	ret.m_position += _steps / 12;
	ret.m_type = static_cast<notetype>(mod(ret.m_type + _steps, 12));

	return ret;
}

note closestNote(float _freq)
{
	int steps = std::round((log(_freq) - logA4) / logTwelthRoot);
	note ret (A, 4);

	ret = ret + steps;

	return ret;
}

//fn = f0 * (a)^s
float freq(note _note)
{
	int s = halfSteps( note(A, 4), _note );
	return 440.0f * pow(twelthRoot, s);
}

int halfSteps(note _a, note _b)
{
	int octaves = (_b.m_position - _a.m_position) * 12;
	int type = (_b.m_type - _a.m_type);
	return octaves + type;
}

bool operator==(const std::vector< notes > &_lhs, const std::vector< notes > &_rhs)
{
	//std::cout << _lhs.size() << " vs " << _rhs.size() << '\n';
	bool r = true;
	for(size_t i = 0; i < _lhs.size(); ++i)
		r = r and similarity(_lhs.at(i), _rhs.at(i), g_PARAM_NOTESET_SIMILARITY_TOLERANCE);
	return r;
}

bool operator==(const notes &_lhs, const notes &_rhs)
{
    return similarity( _lhs, _rhs, g_PARAM_NOTESET_SIMILARITY_TOLERANCE );
}

//Returns true if the vectors are similar enough.
bool similarity(const notes &_lhs, const notes &_rhs, int _tolerance)
{
    int differences = 0;
		if(_lhs.size() == 0 or _rhs.size() == 0)
			return false;
    for(auto &i : _lhs)
    {
        if(std::find(_rhs.begin(), _rhs.end(), i) == _rhs.end())
            differences++;
    }
    for(auto &i : _rhs)
    {
        if(std::find(_lhs.begin(), _lhs.end(), i) == _lhs.end())
            differences++;
    }
    return differences < _tolerance;
}
