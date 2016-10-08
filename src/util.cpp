#include <algorithm>
#include <vector>

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

int levenshtein(const std::string &_a, const std::string &_b)
{
    return 0;
    //Record string lengths.
    /*size_t m = _a.length();
    size_t n = _b.length();

    //Edge case, one or both strings are length zero.
    if(n == 0) return m;
    else if(m == 0) return n;

    //Increment for matrix construction.
    //Matrix dimensions must be one  larger than each of the strings.
    n++;
    m++;

    //Declare matrix
    std::vector<std::vector<size_t>> matrix;

    //For every x...
    for(size_t x = 0; x < m; ++x)
    {
        //Create and dop in column of length n.
        std::vector<size_t> temp;
        temp.assign(n, 0);
        matrix.push_back(temp);
    }

    //Decrement m and n
    n--;
    m--;

    for(size_t x = 0; x < m; ++x)
    {
        for(size_t y = 0; y < n; ++y)
        {

        }
    }*/
}

/*
def lev( _str1, _str2 ):
    m = len( _str1 )
  n = len( _str2 )

  if n == 0:
  return m
  elif m == 0:
  return n

  n += 1
  m += 1

  #Initialise a matrix with 0->m columns and 0->n rows
  matrix = [ [ 0 for x in range(n) ] for y in range(m) ]

  for x in range( m ):
  matrix[x][0] = x
  for y in range( n ):
  matrix[0][y] = y

  n -= 1
  m -= 1

  for i in range( m ):
  mx = i + 1
  for j in range( n ):
  my = j + 1
  cost = 0
  if( _str1[i] != _str2[j] ):
  cost = 1

  minVal = min( matrix[mx][my - 1] + 1, matrix[mx - 1][my] + 1 )
  minVal = min( minVal, matrix[mx - 1][my - 1] + cost	)

  matrix[mx][my] = minVal

  #for i in range(len(matrix)):
  #	print matrix[i]

  return matrix[m][n]
*/

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
