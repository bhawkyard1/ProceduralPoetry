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


template<class T>
class markovState
{
public:
	markovState();

	void addConnection(const T &_id);

	markovEdge<T> getConnection(const size_t _i) const {return m_connections[_i];}
	std::vector<markovEdge<T>> getConnections() const {return m_connections;}
	float getMaxProb() const {return m_maxProbability;}
	size_t getNumConnections() const {return m_connections.size();}

	markovEdge<T> getRandomConnection();
private:
	std::vector<markovEdge<T>> m_connections;
	float m_maxProbability;
};

template<class T>
markovState<T>::markovState()
{
	m_maxProbability = 0.0f;
}

template<class T>
void markovState<T>::addConnection(const T &_id)
{
	//Increase max probability by 1,0f, indicating new entry
	m_maxProbability += 1.0f;

	//If entry is a duplicate, increase probability on this connection, rather than adding a new one.
	//This has the same effect that adding a new connection would, but it uses less memory.
	for(auto &i : m_connections)
		if(i.m_node == _id)
		{
			i.m_probability += 1.0f;
			return;
		}

	//If we make it through the loop (ie the connection is not a duplicate) create a new connection and return.
	m_connections.push_back({_id, 1.0f});
}

template<class T>
markovEdge<T> markovState<T>::getRandomConnection()
{
	//Generate a random number between zero and m_maxProbability
	float p = randFlt(0.0f, m_maxProbability);
	float curProb = 0.0f;

	//Loop through connections...
	//If sum of all previously checked probabilities is less than p, and p is also less than this plus the probability of the current connection
	//We return this connection
	//If not we add current node probability to running total
	for(auto &connection : m_connections)
	{
		if(p > curProb and p < curProb + connection.m_probability)
			return connection;
		curProb += connection.m_probability;
	}

	//Return a null node if all of this fails. A bit shit, but hey.
	//This should ONLY happen if there are no connections.
	std::cerr << "Warning! Node has no connections (markovState::getRandomConnection())\n";
	return {"", -1.0f};
}

#endif
