#ifndef MARKOVCHAIN_HPP
#define MARKOVCHAIN_HPP

#include <map>
#include <string>

#include "markovState.hpp"

class markovChain
{
public:
    markovChain(size_t _order);

    void addNode(const markovState &_node);

    void loadSource( const std::string _path );

    void write();
private:
    size_t m_order;
    std::map<std::string, markovState> m_states;
};

#endif
