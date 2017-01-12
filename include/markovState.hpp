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
