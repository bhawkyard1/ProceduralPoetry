#ifndef MARKOVCHAIN_HPP
#define MARKOVCHAIN_HPP

#include <string>

#include "markovState.hpp"
#include "slotMap.hpp"

class markovChain
{
public:
    markovChain();

    void addNode(const markovState &_node) {m_states.push_back(_node);}

    void writePoem();
private:
    slotMap<markovState> m_states;
};

#endif
