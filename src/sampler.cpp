#include <algorithm>
#include <iostream>
#include <fstream>

#include "sampler.hpp"

#include "fft.hpp"
#include "kiss/kiss_fft.h"

#include "util.hpp"

int sampler::s_sampleRate = 0;
int sampler::s_channels = 2;

void sampler::initialiseAudio(const int _rate, const int _channels)
{
	if( Mix_OpenAudio( _rate, MIX_DEFAULT_FORMAT, 2, 1024 ) < 0)
	{
		std::cout << "Mix_OpenAudio error : " << Mix_GetError() << '\n';
		exit(EXIT_FAILURE);
	}

	s_sampleRate = _rate;
	s_channels = _channels;
}

sampler::sampler(const std::string _path)
{
	m_snd = Mix_LoadWAV(_path.c_str());
	if(!m_snd)
	{
		std::cerr << "Could not load wav file! " << Mix_GetError() << '\n';
		exit(EXIT_FAILURE);
	}

	//Buffer length in bytes / bytes in  an i16 = number of i16s to represent the buffer
	size_t s = m_snd->alen / sizeof(int16_t);
	m_buf.reserve( s );
	for(size_t i = 0; i < m_snd->alen; i += sizeof(int16_t))
		m_buf.push_back( *((uint16_t *)&m_snd->abuf[i]) );

	m_len = m_snd->alen / (float)(s_sampleRate * s_channels * sizeof(int16_t));

	std::cout << "Audio file has len " << m_snd->alen << " bytes.\nWith "
						<< s_channels << " channels, a sample rate of "
						<< s_sampleRate << " and a format of length "
						<< sizeof(int16_t) <<
							 " this means that it is " << m_len << " seconds long.\n";
}

sampler::~sampler()
{
	Mix_FreeChunk(m_snd);
}

std::vector<float> sampler::sampleAudio(const float _start, const int _width)
{
	//Find start index from time. int16 = 2 bytes.
	int startByte = _start * s_sampleRate * s_channels;
	int startIndex = startByte;// / sizeof(int16_t);
	//Clamp it to zero, just in case.
	startIndex = std::max(startIndex, 0);
	//Get end sample using width.
	int endIndex = startIndex + _width / sizeof(int16_t);
	//Get the difference between these (in current implementation this should be equal to _width).
	int len = endIndex - startIndex;

	if(len == 0)
		return {};

	//Create our complex buffer.
	std::valarray<Complex> arr;

	//Reserve memory.
	arr.resize( len, {0.0f, 0.0f} );

	//I don't know if this makes sense, but the buffer is all uint8s and the wav is 16-bit.
	for(size_t i = 0; i < arr.size(); ++i)
	{
		float f = static_cast<float>(m_buf[startIndex + i]);
		Complex insert = { f, 0.0f};
		arr[i] = insert;
	}

	hanning(arr);
	fft(arr);

	std::vector<float> ret;
	ret.reserve( arr.size() / 4 );

	for(size_t i = 0; i < arr.size() / 4; ++i)
	{
		//std::cout << i * ((float)s_sampleRate / arr.size()) << " Hz -> " << mag( arr[i] ) << '\n';
		float l = magns( arr[i] );
		float r = magns( arr[arr.size() / 2 - 1 - i]);
		l = 20 * log10(l);
		r = 20 * log10(r);
		l /= len;
		r /= len;
		ret.push_back( (l + r) / 2 );
	}

	/*auto m = std::max_element( ret.begin() + 1, ret.end() );
		int ind = std::distance( ret.begin(), m );*/
	//std::cout << ind * ((float)s_sampleRate / arr.size()) << " Hz -> " << *m << '\n';

	//std::cout << "p1\n";

	return ret;
}

