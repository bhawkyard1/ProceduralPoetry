//----------------------------------------------------------------------------------------------------------------------
/// \file markovState.hpp
/// \brief This file contains the markov state class, one of the constituent classes of markovChain
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
/// \class markovState
/// \brief The states are the main component of a markov chain. In my case, they are stored in a map in the markovChain.
/// They are indexed by the state contents and the state you see here holds data about the connections. In a way, this
/// class only really represents one half of a markov state, but this configuration makes sense for storage.
//----------------------------------------------------------------------------------------------------------------------

#ifndef MARKOVSTATE_HPP
#define MARKOVSTATE_HPP

#ifdef _WIN32
#include <ciso646>
#endif

#include <algorithm>
#include <iostream>
#include <string>

#include "markovEdge.hpp"
#include "slotmap.hpp"
#include "util.hpp"

class markovState
{
public:
	markovState();

	void addConnection(const notes &_id);

	markovEdge getConnection(const size_t _i) const {return m_connections[_i];}
	std::vector<markovEdge> getConnections() const {return m_connections;}
	float getMaxProb() const {return m_maxProbability;}
	size_t getNumConnections() const {return m_connections.size();}

	markovEdge getRandomConnection();
private:
	std::vector<markovEdge> m_connections;
	float m_maxProbability;
};

#endif
