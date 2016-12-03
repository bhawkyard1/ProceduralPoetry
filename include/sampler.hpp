#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <string>

#include <vector>

#include <SDL2/SDL.h>
#include <SDL_mixer.h>

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
private:
    Mix_Chunk * m_snd;
    std::vector<int16_t> m_buf;
    float m_len;
    static int s_sampleRate;
    static int s_channels;
};

#endif
