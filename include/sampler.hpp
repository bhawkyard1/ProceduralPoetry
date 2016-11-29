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
    sampler(const std::string _path);
    ~sampler();
    Mix_Chunk * get() {return m_snd;}
    float getLenSecs() const {return m_len;}
    std::vector<float> sampleAudio(const float _start, const int _width);
private:
    Mix_Chunk * m_snd;
    std::vector<int16_t> m_buf;
    float m_len;
    static int s_sampleRate;
    static int s_channels;
};

#endif
