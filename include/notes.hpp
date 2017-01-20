//----------------------------------------------------------------------------------------------------------------------
/// \file notes.hpp
/// \brief This file contains code related to the 'notes' class.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
//----------------------------------------------------------------------------------------------------------------------

#ifndef NOTES_HPP
#define NOTES_HPP

#include <cmath>
#include <vector>
#include <string>

#include "common.hpp"

#ifdef _WIN32
#include <ciso646>
#endif

const double twelthRoot = pow(2.0, 1.0 / 12.0);
const double logTwelthRoot = log(twelthRoot);
const double A4 = 440.0;
const double logA4 = log(A4);

enum notetype { C, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B };

notetype operator+(const notetype &_lhs, const int &_rhs);

//----------------------------------------------------------------------------------------------------------------------
/// \struct note
/// \brief A struct representing a musical note, consists of a 'type' (A-G) and a 'position' (ie, octave). There is
/// functionality here to convert between raw frequencies and their closest notes.
//----------------------------------------------------------------------------------------------------------------------
struct note
{
    notetype m_type;
    int m_position;

    note() = default;

    note(notetype _type, int _position)
    {
        m_type = _type;
        m_position = _position;
    }

    bool operator<(const note &_rhs) const
    {
#ifdef _WIN32
        return
                static_cast<int>(m_type) < static_cast<int>(_rhs.m_type) or
                m_position < _rhs.m_position;
#endif

#ifdef __linux__
        if(static_cast<int>(m_type) < static_cast<int>(_rhs.m_type))
            return true;
        else if(static_cast<int>(_rhs.m_type) < static_cast<int>(m_type))
            return false;
        else if(m_position < _rhs.m_position)
            return true;
        else if(_rhs.m_position < m_position)
            return false;
#endif
    }
};

typedef std::vector<note> notes;

bool operator==(const note &_lhs, const note &_rhs);
//bool operator<(const note &_lhs, const note &_rhs);
bool operator>(const note &_lhs, const note &_rhs);
note operator+(const note &_lhs, const int _steps);

note closestNote(float _freq);
float freq(note _note);
int halfSteps(note _a, note _b);

const std::vector<std::string> sNotes = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B" };

bool similarity(const notes &_lhs, const notes &_rhs, int _tolerance);

//----------------------------------------------------------------------------------------------------------------------
/// \struct notesComp
/// \brief A struct simply used to compare two vectors containing notes. This is a critical section of code, it is
/// used to index the sets of notes stored in the markovChains central std::map.
//----------------------------------------------------------------------------------------------------------------------
struct notesComp
{
    bool operator()(const std::vector<notes> &_lhs, const std::vector<notes> &_rhs) const
    {
        for(size_t i = 0; i < _lhs.size(); ++i)
            if( similarity( _lhs[i], _rhs[i], g_PARAM_NOTESET_SIMILARITY_TOLERANCE ) )
                return false;

        return _lhs < _rhs;
    }
};

bool operator==(const std::vector<notes> &_lhs, const std::vector<notes> &_rhs);
bool operator==(const notes &_lhs, const notes &_rhs);

#endif
