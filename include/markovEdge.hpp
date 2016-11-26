#ifndef MARKOVEDGE_HPP
#define MARKOVEDGE_HPP

#include <string>

template<class T>
struct markovEdge
{
    T m_node;
    float m_probability;
};

template<class T>
bool operator==(const markovEdge<T> &_lhs, const markovEdge<T> &_rhs)
{
    return _lhs.m_node == _rhs.m_node;
}

#endif
