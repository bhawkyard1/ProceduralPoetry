#include <ciso646>

#include "slotmap.hpp"

bool operator ==(const slot &_lhs, const slot &_rhs)
{
    return _lhs.m_id == _rhs.m_id and _lhs.m_version == _rhs.m_version;
}

bool operator !=(const slot &_lhs, const slot &_rhs)
{
    return _lhs.m_id != _rhs.m_id or _lhs.m_version != _rhs.m_version;
}
