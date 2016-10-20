#ifndef MATRICES_HPP
#define MATRICES_HPP

#include <algorithm>
#include <ciso646>
#include <iostream>
#include <stddef.h>
#include <string>

#include "util.hpp"

void ps(std::string str);
template<typename T>
T ** constructMatrix(size_t _w, size_t _h);
template<typename T, typename B>
void fillMatrix(
        T ** _mat,
        size_t _w,
        size_t _h,
        B _val
        );
template<typename T>
void cpyMatrix(
        T ** _src,
        T ** _dst,
        size_t _srcw,
        size_t _srch,
        size_t _dstw,
        size_t _dsth);
template<typename T>
void clearMatrix(T ** _mat, size_t _major, size_t _minor);

template<class ptype> class matrix
{
public:
    ptype * operator [ ] (const size_t &_rhs) { return m_digits[_rhs]; }
    ptype * at(const size_t &_rhs) const {return m_digits[_rhs];}

    matrix();
    matrix(size_t _width, size_t _height);
    matrix(const matrix<ptype> &_m);
    ~matrix();
    void setWidth(size_t _w);
    void setHeight(size_t _h);
    void setDim(size_t _w, size_t _h);

    void clear();

    void flood(ptype _v);

    size_t getWidth() const {return m_width;}
    size_t getHeight() const {return m_height;}
    size_t getArea() const {return m_width * m_height;}template<typename T, typename B>
    void fillMatrix(
            T ** _mat,
            size_t _w,
            size_t _h,
            B _val
            )
    {
        for(size_t i = 0; i < _w; ++i)
        {
            std::fill(&_mat[i][0], &_mat[i][0] + _h, _val);
        }
    }

    template<typename T>
    void cpyMatrix(
            T ** _src,
            T ** _dst,
            size_t _srcw,
            size_t _srch,
            size_t _dstw,
            size_t _dsth)
    {
        //We will not copy too much data.
        //This guarantees that the source dimensions will not exceed those of the destination.
        size_t copyWidth = std::min(_srcw, _dstw);
        size_t copyHeight = std::min(_srch, _dsth);

        for(size_t i = 0; i < copyWidth; ++i)
        {
            for(size_t j = 0; j < copyHeight; ++j)
                _dst[i][j] = _src[i][j];
        }
    }

    template<typename T>
    void clearMatrix(T ** _mat, size_t _major, size_t _minor)
    {
        if(_minor > 0)
        {
            for(size_t i = 0; i < _major; ++i)
            {
                delete [] _mat[i];
                _mat[i] = nullptr;
            }
        }
        if(_mat != nullptr) delete [] _mat;
        _mat = nullptr;
    }

    void transpose();

    void makeUnit();

    matrix<ptype> getMinor(int _i, int _j, int _q);

    matrix<ptype> & operator=(const matrix<ptype> &_rhs)
    {
        size_t w = _rhs.getWidth();
        size_t h = _rhs.getHeight();
        //setWidth(w);
        //setHeight(h);
        setDim( w, h );
        cpyMatrix(_rhs.m_digits, m_digits, _rhs.getWidth(), _rhs.getHeight(), getWidth(), getHeight());
        return *this;
    }

private:
    size_t m_width;
    size_t m_height;
    //All the numbers inside the matrix
    ptype * * m_digits;

    void floodSub(int _entry, int _w, ptype _v);
    void deleteDigits();
};

//Operators
template<class ptype>
matrix<ptype> operator + (matrix<ptype> &_lhs, matrix<ptype> &_rhs)
{
    int w = _lhs.getWidth(), h = _lhs.getHeight();

    if(w != _rhs.getWidth() or h != _rhs.getHeight())
    {
        std::cerr << "Incompatible matrix dimensions for addition.\n";
        exit(EXIT_FAILURE);
    }

    matrix<ptype> ret (w, h);

    for(int j = 0; j < h; ++j)
    {
        for(int i = 0; i < w; ++i)
        {
            ret[i][j] = _lhs[i][j] + _rhs[i][j];
        }
    }

    return ret;
}

template<class ptype>
matrix<ptype> operator -(matrix<ptype> &_lhs, matrix<ptype> &_rhs)
{
    int w = _lhs.getWidth(), h = _lhs.getHeight();

    if(w != _rhs.getWidth() or h != _rhs.getHeight())
    {
        std::cerr << "Incompatible matrix dimensions for subtraction.\n";
        exit(EXIT_FAILURE);
    }

    matrix<ptype> ret (w, h);

    for(int j = 0; j < h; ++j)
    {
        for(int i = 0; i < w; ++i)
        {
            ret[i][j] = _lhs[i][j] - _rhs[i][j];
        }
    }

    return ret;
}

template<class ptype>
matrix<ptype> operator *(matrix<ptype> &_lhs, matrix<ptype> &_rhs)
{
    //Width of final matrix.
    size_t w = _lhs.getHeight();
    //Height of final matrix.
    size_t h = _lhs.getWidth();

    //If rows != columns, error out.
    if(_lhs.getWidth() != _rhs.getHeight())
    {
        std::cerr << "Incompatible matrix dimensions for multiplication!\n";
        exit(EXIT_FAILURE);
    }

    matrix<ptype> ret (w, h);

    for(size_t i = 0; i < w; ++i)
    {
        for(size_t j = 0; j < h; ++j)
        {
            for(size_t k = 0; k < h; ++k)
                ret[i][j] = _lhs[k][j] * _rhs[i][k];
        }
    }

    return ret;
}



template<class ptype>
matrix<ptype>::matrix()
{
    m_width = 0;
    m_height = 0;

    m_digits = nullptr;
}

//Make a new matrix.
template<class ptype>
matrix<ptype>::matrix(size_t _width, size_t _height)
{
    m_width = 0;
    m_height = 0;
    m_digits = constructMatrix<ptype>(_width, _height);
    m_width = _width;
    m_height = _height;

    makeUnit();
}

template<class ptype>
matrix<ptype>::matrix(const matrix<ptype> &_m)
{
    m_width = 0;
    m_height = 0;

    m_digits = new ptype * [_m.getHeight()];

    setWidth(_m.getWidth());
    setHeight(_m.getHeight());

    makeUnit();

    cpyMatrix(_m.m_digits, m_digits, _m.getWidth(), _m.getHeight(), getWidth(), getHeight());
}

template<class ptype>
matrix<ptype>::~matrix()
{
    deleteDigits();
}

template<class ptype>
void matrix<ptype>::setDim(size_t _w, size_t _h)
{
    ptype ** tempDigits = constructMatrix<ptype>(_w, _h);
    fillMatrix(tempDigits, _w, _h, 0);
    cpyMatrix(m_digits, tempDigits, m_width, m_height, _w, _h);
    deleteDigits();
    m_digits = tempDigits;
    m_width = _w;
    m_height = _h;
}

template<class ptype>
void matrix<ptype>::setWidth(size_t _w)
{
    ptype ** tempDigits = constructMatrix<ptype>(_w, m_height);
    fillMatrix(tempDigits, _w, m_height, 0);
    cpyMatrix(m_digits, tempDigits, m_width, m_height, _w, m_height);
    deleteDigits();
    m_digits = tempDigits;
    m_width = _w;
}

template<class ptype>
void matrix<ptype>::setHeight(size_t _h)
{
    ptype ** tempDigits = constructMatrix<ptype>(m_width, _h);
    fillMatrix(tempDigits, m_width, _h, 0);
    cpyMatrix(m_digits, tempDigits, m_width, m_height, m_width, _h);
    deleteDigits();
    m_digits = tempDigits;
    m_height = _h;
}

template<class ptype>
void matrix<ptype>::makeUnit()
{
    flood(0);
    for(size_t i = 0; i < m_width; ++i)
    {
        for(size_t j = 0; j < m_height; ++j)
        {
            if(i == j)
            {
                m_digits[i][j] = 1;
            }
        }
    }
}

template<class ptype>
void matrix<ptype>::flood(ptype _v)
{
    /*
     * This function makes all the contents of your matrix = v
     */

   /* for(size_t i = 0; i < m_width; ++i)
    {
       for(size_t j = 0; j < m_height; ++j)
           m_digits[i][j] = _v;
    }*/


    for(size_t i = 0; i < m_width; ++i)
        std::fill(&m_digits[i][0], &m_digits[i][0] + m_height, _v);

}

template<class ptype>
void matrix<ptype>::floodSub(int _entry, int _w, ptype _v)
{
    /*
     * This is the same as flood, but just for one of the sub-arrays.
     */

    for(int i = 0; i < _w; ++i)
    {
        m_digits[_entry][i] = _v;
    }
}

//No idea what this is for.
template<class ptype>
void matrix<ptype>::transpose()
{
    ptype ** flip = constructMatrix<ptype>(m_height, m_width);
    for(size_t i = 0; i < m_width; ++i)
    {
        for(size_t j = 0; j < m_height; ++j)
            flip[j][i] = m_digits[i][j];
    }
    deleteDigits();
    m_digits = flip;
    std::swap(m_width, m_height);
}

template<class ptype>
matrix<ptype> matrix<ptype>::getMinor(int _i, int _j, int _q)
{
    if(_q < 0) _q = 0;
    else if(_q > 3) _q = 3;

    //quadrant:
    //	0X1
    //	XXX
    //	2X3

    //Return matrix construction
    matrix rm(_i , _j);
    int xadd = 0, yadd = 0;

    if( _q == 1 )
    {
        rm.setDim( _i - 1 - m_width, _j );
        return rm;
        xadd = _i;
    }
    else if( _q == 2 )
    {
        rm.setDim( _i, m_height - 1 - _j );
        yadd = _j + 1;
    }
    else if( _q == 3 )
    {
        return rm;
        std::cout << "q3" << std::endl;
        rm.setDim( m_width - 1 - _i, m_height - 1 - _j );
        xadd = _i;
        yadd = _j + 1;
    }

    std::cout << "CONDITION" << std::endl;

    if(rm.getArea() == 0) return rm;

    //Fill rm
    std::cout << "fill" << std::endl;
    for(int y = 0; y < rm.getHeight(); ++y)
    {
        for(int x = 0; x < rm.getWidth(); ++x)
        {
            rm[x][y] = m_digits[x + xadd][y + yadd];
        }
    }

    return rm;
}

template<class ptype>
matrix<ptype> hadamardMultiply(matrix<ptype> &_a, matrix<ptype> &_b)
{
    if(_a.getWidth() != _b.getWidth() or _a.getHeight() != _b.getHeight())
        return matrix<ptype>();

    matrix<ptype> ret (_a);

    for(size_t i = 0; i < ret.getWidth(); ++i)
    {
        for(size_t j = 0; j < ret.getHeight(); ++j)
        {
            ret[i][j] = _a[i][j] * _b[i][j];
        }
    }

    return ret;
}

template<class ptype>
matrix<ptype> horizontalConcatenation(const matrix<ptype> &_a, const matrix<ptype> &_b)
{
    if(_a.getHeight() != _b.getHeight()) return matrix<ptype> (0,0);

    matrix<ptype> ret (_a.getWidth() + _b.getWidth(), _a.getHeight(), _b.getHeight());
    for(size_t i = 0; i < _a.getWidth(); ++i)
    {
        std::copy(&_a[i][0], &_a[i][0] + _a.getHeight(), &ret[i][0]);
    }
    for(size_t i = 0; i < _b.getWidth(); ++i)
    {
        std::copy(&_b[i][0], &_b[i][0], &ret[i + _a.getWidth()][0]);
    }

    return ret;
}


template<class ptype>
void printMatrix(matrix<ptype> m)
{
    std::cout << "AREA " << m.getArea() << std::endl;
    if(m.getArea() == 0) return;

    for(int j = 0; j < m.getHeight(); ++j)
    {
        for(int i = 0; i < m.getWidth(); ++i)
        {
            std::cout << m[i][j] << "	";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

template<class ptype>
void matrix<ptype>::deleteDigits()
{
    clearMatrix(m_digits, m_width, m_height);
}


template<typename T>
T ** constructMatrix(size_t _w, size_t _h)
{
    if(_w <= 0)
        return nullptr;
    T ** ret = new T * [_w];
    for(size_t i = 0; i < _w; ++i)
    {
        if(_h > 0) ret[i] = new T [_h];
        else ret[i] = nullptr;
    }
    return ret;
}

template<typename T, typename B>
void fillMatrix(
        T ** _mat,
        size_t _w,
        size_t _h,
        B _val
        )
{
    for(size_t i = 0; i < _w; ++i)
    {
        std::fill(&_mat[i][0], &_mat[i][0] + _h, _val);
    }
}

template<typename T>
void cpyMatrix(
        T ** _src,
        T ** _dst,
        size_t _srcw,
        size_t _srch,
        size_t _dstw,
        size_t _dsth)
{
    //We will not copy too much data.
    //This guarantees that the source dimensions will not exceed those of the destination.
    size_t copyWidth = std::min(_srcw, _dstw);
    size_t copyHeight = std::min(_srch, _dsth);

    for(size_t i = 0; i < copyWidth; ++i)
    {
        for(size_t j = 0; j < copyHeight; ++j)
            _dst[i][j] = _src[i][j];
    }
}

template<typename T>
void clearMatrix(T ** _mat, size_t _major, size_t _minor)
{
    if(_minor > 0)
    {
        for(size_t i = 0; i < _major; ++i)
        {
            delete [] _mat[i];
            _mat[i] = nullptr;
        }
    }
    if(_mat != nullptr) delete [] _mat;
    _mat = nullptr;
}

#endif
