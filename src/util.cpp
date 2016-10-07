#include <vector>

#include "util.hpp"

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
