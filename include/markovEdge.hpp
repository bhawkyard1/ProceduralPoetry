//----------------------------------------------------------------------------------------------------------------------
/// \file markovEdge.hpp
/// \brief This file contains the markov edge class, one of the constituent classes of markovState
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
/// \class markovEdge
/// \brief Represents an outbound connection from a markov state. m_node is the data it connects to, m_probability is the
/// (un-normalised) probability of this edge being chosen.
//----------------------------------------------------------------------------------------------------------------------

#ifndef MARKOVEDGE_HPP
#define MARKOVEDGE_HPP

#include <string>

#include "notes.hpp"

struct markovEdge
{
		notes m_node;
    float m_probability;
};

bool operator==(const markovEdge &_lhs, const markovEdge &_rhs);

#endif
