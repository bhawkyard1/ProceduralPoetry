//----------------------------------------------------------------------------------------------------------------------
/// \file fft.hpp
/// \brief This file contains functions related to the fast fourier transform, fft() was sourced online, hanning() was
/// created from online documentation.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// CITATIONS
///
/// Fast Fourier transform. Available from: https://rosettacode.org/wiki/Fast_Fourier_transform [Accessed 19 January 2017].
///
/// National Instruments, 2016. Understanding FFTs and Windowing.
//----------------------------------------------------------------------------------------------------------------------

#ifndef FFT_HPP
#define FFT_HPP

#include <complex>
#include <iostream>
#include <valarray>

const float PI = 3.141592653589793238460;

typedef std::complex<float> Complex;
typedef std::valarray<Complex> CArray;

// Cooley–Tukey FFT (in-place, divide-and-conquer)
/// Sourced from : Fast Fourier transform. Available from: https://rosettacode.org/wiki/Fast_Fourier_transform [Accessed 19 January 2017].
void fft(CArray& _x);

/// Based on : National Instruments, 2016. Understanding FFTs and Windowing.
void hanning(CArray &_x);

float magns(Complex &_x);

float mag(Complex &_x);

#endif
