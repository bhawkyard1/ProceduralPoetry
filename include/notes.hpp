#ifndef NOTES_HPP
#define NOTES_HPP

#include <cmath>
#include <vector>
#include <string>

#ifdef _WIN32
#include <ciso646>
#endif

const double twelthRoot = pow(2.0, 1.0 / 12.0);
const double logTwelthRoot = log(twelthRoot);
const double A4 = 440.0;
const double logA4 = log(A4);

enum notetype { A, Bb, B, C, Db, D, Eb, E, F, Gb, G, Ab };

notetype operator+(const notetype &_lhs, const int &_rhs);

struct note
{
	notetype m_type;
	int m_position;

	note(notetype _type, int _position)
	{
		m_type = _type;
		m_position = _position;
	}

	bool operator<(const note &_rhs) const { return m_position < _rhs.m_position or static_cast<int>(m_type) < static_cast<int>(_rhs.m_type); }
};

bool operator==(const note &_lhs, const note &_rhs);
//bool operator<(const note &_lhs, const note &_rhs);
bool operator>(const note &_lhs, const note &_rhs);
note operator+(const note &_lhs, const int _steps);

note closestNote(float _freq);
float freq(note _note);
int halfSteps(note _a, note _b);

const std::vector<std::string> sNotes = {"A", "Bb", "B", "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab" };

typedef std::vector<note> notes;

#endif
