#include "fft.hpp"

// Cooley–Tukey FFT (in-place, divide-and-conquer)
// Higher memory requirements and redundancy although more intuitive
void fft(CArray& _x)
{
    const size_t N = _x.size();
    if (N <= 1) return;

    // divide
    CArray even = _x[std::slice(0, N/2, 2)];
    CArray  odd = _x[std::slice(1, N/2, 2)];

    // conquer
    fft(even);
    fft(odd);

    // combine
    for (size_t k = 0; k < N/2; ++k)
    {
        Complex t = std::polar(1.0f, static_cast<float>(-2 * PI * k / N)) * odd[k];
        _x[k    ] = even[k] + t;
        _x[k+N/2] = even[k] - t;
    }
}

float magns(Complex &_x)
{
    return _x.real() * _x.real() + _x.imag() * _x.imag();
}

void hanning(CArray &_x)
{
    for(size_t i = 0; i < _x.size(); ++i)
    {
        float mul = 0.5f * (1 - cos(2 * PI * i / _x.size()));
        _x[i] = mul * _x[i];
    }
}

float mag(Complex &_x)
{
    return sqrt(_x.real() * _x.real() + _x.imag() * _x.imag());
}
