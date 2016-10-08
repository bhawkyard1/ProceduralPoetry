#ifndef MARKOVSTATE_HPP
#define MARKOVSTATE_HPP

#include <string>

#include "markovEdge.hpp"
#include "slotMap.hpp"

class markovState
{
public:
    markovState();

    void addConnection(const std::string &_id);

    markovEdge getConnection(const size_t _i) const {return m_connections[_i];}
    std::vector<markovEdge> getConnections() const {return m_connections;}

    markovEdge getRandomConnection();
private:
    std::vector<markovEdge> m_connections;
    float m_maxProbability;
};

#endif
