#include <ciso646>

#include "slotMap.hpp"

bool operator ==(const slotID &_lhs, const slotID &_rhs)
{
    return _lhs.m_id == _rhs.m_id and _lhs.m_version == _rhs.m_version;
}

bool operator !=(const slotID &_lhs, const slotID &_rhs)
{
    return _lhs.m_id != _rhs.m_id or _lhs.m_version != _rhs.m_version;
}
