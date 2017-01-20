//----------------------------------------------------------------------------------------------------------------------
/// \file sampler.hpp
/// \brief This file contains the 'sampler' class, and related types and some global variables.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
/// \class sampler
/// \brief A class that can load and analyse an audio file. Uses the functions defined in fft.hpp to do so.
//----------------------------------------------------------------------------------------------------------------------

#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <string>

#include <vector>

#include <SDL2/SDL.h>
#include <SDL_mixer.h>

#include "notes.hpp"

extern std::vector<std::vector<float>> g_noteIntensity;
extern int g_noteIntensityOrder;
extern std::vector<float> g_averageNoteIntensity;

class sampler
{
public:
		static void initialiseAudio(const int _rate, const int _channels);
		static int getSampleRate() {return s_sampleRate;}
		static int getChannels() {return s_channels;}
		sampler() = default;
		sampler(const std::string _path);
		~sampler();
		void load(const std::string _path);
		Mix_Chunk * get() {return m_snd;}
		float getLenSecs() const {return m_len;}
		float bytesToSecs(const int _i) {return _i / (float)(s_sampleRate * s_channels);}
		int secsToBytes(const float _i) {return std::ceil(_i * s_sampleRate * s_channels * sizeof(int16_t));}
		std::vector<float> sampleAudio(const float _start, const int _width);
		void reset() {Mix_FreeChunk(m_snd);}
		static int s_sampleRate;
		static int s_channels;
private:
		Mix_Chunk * m_snd;
		std::vector<int16_t> m_buf;
		float m_len;
};

std::vector<float> getNoteVals(const std::vector<float> &_freq);
std::vector<note> getActiveNotes(const std::vector<float> &_intensities);
std::vector<note> getActiveNotes(const std::vector<float> &_intensities, float *_ints);

#endif
