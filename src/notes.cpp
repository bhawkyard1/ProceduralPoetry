#include <cmath>

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

note closestNote(float _freq)
{
	int steps = std::round((log(_freq) - logA4) / logTwelthRoot);
	note ret (A, 4);

	ret.m_position += steps / 12;

	ret.m_type = ret.m_type + mod(steps, 12);

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

