#ifndef FFT_HPP
#define FFT_HPP

#include <complex>
#include <iostream>
#include <valarray>

const float PI = 3.141592653589793238460;

typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;

// Cooley–Tukey FFT (in-place, divide-and-conquer)
// Higher memory requirements and redundancy although more intuitive
void fft(CArray& _x);

void hanning(CArray &_x);

float magns(Complex &_x);

float mag(Complex &_x);

#endif
