#ifndef MARKOVEDGE_HPP
#define MARKOVEDGE_HPP

#include <string>

#include "notes.hpp"

struct markovEdge
{
		notes m_node;
    float m_probability;
};

bool operator==(const markovEdge &_lhs, const markovEdge &_rhs);

#endif
