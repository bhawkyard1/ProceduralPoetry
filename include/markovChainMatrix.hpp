#ifndef MARKOVCHAIN_HPP
#define MARKOVCHAIN_HPP

#include <map>
#include <string>

#include "matrices.hpp"
#include "slotmap.hpp"

class markovChain
{
public:
    markovChain(size_t _order);
private:
    std::map< std::string, size_t > m_indices;
    matrix<float> m_connections;
};

#endif
