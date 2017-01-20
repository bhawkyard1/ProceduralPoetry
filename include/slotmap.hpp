//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file contains work developed as a part of
// my Year 2 Computing for Animation 1 project, with
// some large alterations to functionality. It
// may not be appropriate to consider it.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//----------------------------------------------------------------------------------------------------------------------
/// \file slotmap.hpp
/// \brief This file contains the 'slotmap' class, and related structs.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
/// \class slotmap
/// \brief A wrapper around stl std::vector which allows the user to extract persistant references to entities
/// contained within.
//----------------------------------------------------------------------------------------------------------------------

#ifndef slotmap_HPP
#define slotmap_HPP

#ifdef _WIN32
#include <ciso646>
#endif
#include <stddef.h>
#include <vector>

struct slot
{
    slot()
    {
        m_id = 0;
        m_version = -1;
    }

    slot(const long _id, const long _version)
    {
        m_id = _id;
        m_version = _version;
    }

    long m_id;
    long m_version;
};

bool operator ==(const slot &_lhs, const slot &_rhs);
bool operator !=(const slot &_lhs, const slot &_rhs);

template<class t>
class slotmap
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Vector of raw objects.
    //----------------------------------------------------------------------------------------------------------------------
    std::vector<t> m_objects;

    t * getByID(slot _i)
    {
        //Something wrong here. m_indirection not keeping up with m_ids.
        if(_i.m_id < m_indirection.size() and m_indirection[ _i.m_id ].m_version == _i.m_version)
            return &m_objects[ m_indirection[ _i.m_id ].m_id ];
        return nullptr;
    }

    slot push_back(const t &_obj)
    {
        slot ret;
        //If there are free spaces...
        if(m_freeList.size() > 0)
        {
            long freeid = m_freeList.back();
            long ver = m_indirection[ freeid ].m_version;

            m_indirection[ freeid ].m_id = m_objects.size();
            m_ids.push_back( {freeid, ver} );
            m_freeList.pop_back();

            ret = {freeid, ver};
        }
        //Create a new object, a new id and a new entry in the indirection list.
        else
        {
            slot id = {static_cast<long>(m_objects.size()), 0};
            m_indirection.push_back( id );
            m_ids.push_back( id );

            ret = id;
        }

        m_objects.push_back(_obj);

        return ret;
    }

    //Swaps item at index _a with item at index _b
    void swap(long _a, long _b)
    {
        if(_a == _b) return;

        //Store entry pointed to by the id of _a
        slot swap = {_a, m_ids[_b].m_version};

        //Make the entry at _a's id point to _a's future index.
        m_indirection[ m_ids[_a].m_id ] = {_b, m_ids[_a].m_version};
        //Make the entry at _b's id point to _b's future index.
        m_indirection[ m_ids[_b].m_id ] = swap;

        //std::swap( m_objects[_a], m_objects[_b] );
        //std::swap( m_ids[_a], m_ids[_b] );

        iter_swap( m_objects.begin() + _a, m_objects.begin() + _b );
        iter_swap( m_ids.begin() + _a, m_ids.begin() + _b );
    }

    void pop()
    {
        //Add id to freelist.
        m_freeList.push_back( m_ids.back().m_id );
        //Increment version on indirection list.
        m_indirection[ m_ids.back().m_id ].m_version += 1;

        //Destroy ID.
        m_ids.pop_back();
        //Destroy object.
        m_objects.pop_back();
    }

    void free(size_t _i)
    {
        swap(_i, m_objects.size() - 1);
        pop();
    }

    t& back() const {return m_objects.back();}
    t& back() {return m_objects.back();}
    slot backID() {return m_ids.back();}

    void clear()
    {
        m_objects.clear();
        m_ids.clear();
        m_indirection.clear();
        m_freeList.clear();
    }

    size_t size() const {return m_objects.size();}

    slot getID(size_t _i) const {return m_ids[_i];}
    size_t getIndex(slot _id) const {return m_indirection[ _id.m_id ].m_id;}

    t operator [](size_t _i) const {return m_objects[_i];}
    t & operator [](size_t _i) {return m_objects[_i];}

    t get(size_t _i) const {return m_objects[_i];}
    t & get(size_t _i) {return m_objects[_i];}

private:
    //----------------------------------------------------------------------------------------------------------------------
    /// \brief The index of each entry is the id of the object. The contents id is the index of the object. The version is the version of the object.
    /// Confused? Me too.
    //----------------------------------------------------------------------------------------------------------------------
    std::vector< slot > m_indirection;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief Means we do not have to store ids in the object, this matches movements of m_objects by index.
    //----------------------------------------------------------------------------------------------------------------------
    std::vector< slot > m_ids;

    //----------------------------------------------------------------------------------------------------------------------
    /// \brief List of all free IDs.
    //----------------------------------------------------------------------------------------------------------------------
    std::vector< long > m_freeList;
};

template<class t>
void transfer(size_t _i, slotmap<t> &_src, slotmap<t> &_dst)
{
    _dst.push_back( _src[_i] );
    _src.free( _i );
}

template<class T>
struct slotID : public slot
{
    slotID() : slot()
    {
        m_address = nullptr;
    }

    slotID(const long _id, const long _version, slotmap<T> * _src) : slot(_id, _version)
    {
        m_address = _src;
    }

    slotmap<T> * m_address;
};

#endif
