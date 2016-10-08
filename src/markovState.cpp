#include <algorithm>
#include <iostream>

#include "markovState.hpp"

#include "util.hpp"

#ifdef _WIN32
#include <ciso646>
#endif

markovState::markovState()
{
    m_maxProbability = 0.0f;
}

void markovState::addConnection(const std::string &_id)
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

markovEdge markovState::getRandomConnection()
{
    //Generate a random number between zero and m_maxProbability
    float p = randNum(0.0f, m_maxProbability);
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
    std::cerr << "Warning! Null node being returned from markovState::getRandomConnection()\n";
    return {{0, -1}, -1.0f};
}

