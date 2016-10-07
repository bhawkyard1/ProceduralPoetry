#ifndef MARKOVSTATE_HPP
#define MARKOVSTATE_HPP

#include <string>

#include "markovEdge.hpp"
#include "slotMap.hpp"

class markovState
{
public:
    markovState(const std::string &_contents);

    void addConnection(slotID _id);

    markovEdge getConnection(const size_t _i) const {return m_connections[_i];}
    std::vector<markovEdge> getConnections() const {return m_connections;}

    markovEdge getRandomConnection();

    std::string getContents() const {return m_contents;}
private:
    std::vector<markovEdge> m_connections;
    std::string m_contents;

    slotID m_id;

    float m_maxProbability;
};

#endif
