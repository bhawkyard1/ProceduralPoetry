#ifndef RANGE_HPP
#define RANGE_HPP

template<class T>
class range
{
public:
    range(T _low, T _high)
    {
        m_low = _low;
        m_high = _high;
    }

    bool inInclusive(T _val)
    {
        return _val >= m_low and _val <= m_high;
    }

    bool inNonInclusive(T _val)
    {
        return _val > m_low and _val < m_high;
    }

    bool operator<(const T _rhs) {return inNonInclusive(_rhs);}
    bool operator<<(const T _rhs) {return inInclusive(_rhs);}

private:
    T m_low;
    T m_high;
};

#endif
