#ifndef MARKOVCHAIN_HPP
#define MARKOVCHAIN_HPP

#include <map>
#include <queue>
#include <string>

#include "markovState.hpp"

class markovChain
{
public:
    markovChain(size_t _order);

    void addNode(const markovState &_node);

    void diagnoseNode( const std::string &_str );

    float getAverageNumConnections();

    void loadSource( const std::string _path );

    void write(size_t _wordCount);

    void reload(int _order);
    size_t getOrder() {return m_order;}
private:
    size_t m_order;

    //bool m_permitOrderDegeneration;

    //We write into this buffer, and can write to disk/display to screen.
    std::string m_writeBuffer;

    //Traversing m_states will only return one string per step. This is written into a buffer (FIFO queue) to give context for the next step.
    std::deque<std::string> m_seekBuffer;
    //This is the guts of the chain, the vector of strings (which will be order in length) is the string associates
    std::map<std::vector<std::string>, markovState> m_states;

    void addContext(const std::string &_str);
    std::vector<std::string> getKeyFromContext();
    void resetBuffers() {m_writeBuffer = ""; m_seekBuffer.clear();}

    void load();
};

#endif
