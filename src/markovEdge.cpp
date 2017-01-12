#include "markovEdge.hpp"

bool operator==(const markovEdge &_lhs, const markovEdge &_rhs)
{
		return _lhs.m_node == _rhs.m_node;
}
