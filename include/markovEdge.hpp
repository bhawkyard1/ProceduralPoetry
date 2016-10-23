#ifndef MARKOVEDGE_HPP
#define MARKOVEDGE_HPP

#include <string>

struct markovEdge
{
    std::string m_node;
    float m_probability;
};

bool operator==(const markovEdge &_lhs, const markovEdge &_rhs);

#endif
