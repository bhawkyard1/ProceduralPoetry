#include <algorithm>
#include <vector>
#include <SDL2/SDL.h>
#include <ngl/NGLStream.h>

#include "matrices.hpp"
#include "util.hpp"

std::istream& getlineSafe(std::istream& is, std::string& t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}

ngl::Vec2 getMousePos()
{
    int x = 0;
    int y = 0;
    SDL_GetMouseState( &x, &y );

    return ngl::Vec2( x, y );
}

ngl::Vec2 getMousePosWindowCorrective(SDL_Window * _window)
{
    ngl::Vec2 mouse = getMousePos();
    int w = 0, h = 0;

    SDL_GetWindowSize( _window, &w, &h );
    ngl::Vec2 fakeDimensions ( w, h );

    SDL_GL_GetDrawableSize( _window, &w, &h );
    ngl::Vec2 realDimensions ( w, h );

    std::cout << "Fake " << fakeDimensions << " Real " << realDimensions << '\n';

    ngl::Vec2 offset = fakeDimensions - realDimensions;
    mouse -= offset;

    return mouse;
}

int levenshtein(const std::string &_a, const std::string &_b)
{
    //Record string lengths.
    size_t m = _a.length();
    size_t n = _b.length();

    //Edge case, one or both strings are length zero.
    if(n == 0) return m;
    else if(m == 0) return n;

    //Increment for matrix construction.
    //Matrix dimensions must be one  larger than each of the strings.
    n++;
    m++;

    //Declare matrix
    matrix<size_t> mat (m, n);
    for(size_t i = 0; i < m; ++i)
        mat[i][0] = i;
    for(size_t j = 0; j < n; ++j)
        mat[0][j] = j;

    //Decrement m and n
    n--;
    m--;

    for(size_t x = 0; x < m; ++x)
    {
        size_t mx = x + 1;
        for(size_t y = 0; y < n; ++y)
        {
            size_t my = y +1;

            size_t cost = 0;
            if(_a[x] != _b[y])
                cost = 1;

            size_t min = std::min( mat[mx][my - 1] + 1, mat[mx - 1][my] + 1 );
            min = std::min( min, mat[mx - 1][my - 1] + cost);

            mat[mx][my] = min;
        }
    }

    return mat[m][n];
}

std::pair<std::string, std::string> splitFirst(std::string _str, char _delim)
{
    std::pair<std::string, std::string> ret;
    bool section = false;

    for(auto &ch : _str)
    {
        if(ch == _delim)
            section = true;

        if(!section)
            ret.first += ch;
        else
            ret.second += ch;
    }

    return ret;
}

std::vector<std::string> split(std::string _str, char _delim)
{
    std::vector<std::string> ret;

    std::stringstream ss(_str);
    std::string sub;

    while( getline( ss, sub, _delim ) )
    {
        if(sub != "")
            ret.push_back(sub);
    }

    return ret;
}

float rad(const float _deg)
{
    return _deg * (M_PI / 180.0f);
}


std::vector<std::string> split(std::string _str, std::string _delims)
{
    std::vector<std::string> ret;

    for(auto &character : _delims)
    {
        std::replace(_str.begin(), _str.end(), character, _delims[0]);
    }

    std::stringstream ss(_str);
    std::string sub;

    while( getline( ss, sub, _delims[0] ) )
    {
        if(sub != "")
            ret.push_back(sub);
    }

    return ret;
}

void errorExit(const std::string &_msg)
{
    std::cerr << &_msg << '\n';
    exit(EXIT_FAILURE);
}

int randInt(int _low, int _high)
{
    if( _low == _high )
        return _low;

    std::uniform_int_distribution< int > uni( _low, _high );
    return uni( g_RANDOM_TWISTER );
}

float randFlt(float _low, float _high)
{
    if( _low == _high )
        return _low;

    std::uniform_real_distribution< float > uni( _low, _high );
    return uni( g_RANDOM_TWISTER );
}

std::pair<ngl::Vec3, ngl::Vec3> lim(const std::vector<ngl::Vec3> &_vecs)
{
    std::pair<ngl::Vec3, ngl::Vec3> ret;
    ret.first = ngl::Vec3( F_MAX, F_MAX, F_MAX );
    ret.second = ngl::Vec3( -F_MAX, -F_MAX, -F_MAX );

    for(auto &_vec : _vecs)
    {
        ret.first.m_x = std::min( ret.first.m_x, _vec.m_x );
        ret.first.m_y = std::min( ret.first.m_y, _vec.m_y );
        ret.first.m_z = std::min( ret.first.m_z, _vec.m_z );

        ret.second.m_x = std::max( ret.second.m_x, _vec.m_x );
        ret.second.m_y = std::max( ret.second.m_y, _vec.m_y );
        ret.second.m_z = std::max( ret.second.m_z, _vec.m_z );
    }

    return ret;
}

std::string toString(const std::vector<std::string> &_vec)
{
    std::string ret = "";
    for(auto &i : _vec)
        ret += i + ' ';
    ret.pop_back();
    return ret;
}

//http://stackoverflow.com/questions/1082917/mod-of-negative-number-is-melting-my-brain
int mod(int x, int m)
{
		int r = x%m;
		return r<0 ? r+m : r;
}
