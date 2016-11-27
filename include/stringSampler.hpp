#ifndef STRINGSAMPLER_HPP
#define STRINGSAMPLER_HPP

#include <vector>
#include <map>
#include <string>

#include "markovChain.hpp"
#include "sampler.hpp"

class stringSampler : public sampler
{
public:
    void sample(const std::string &_source, markovChain<std::string> * _chain);
private:
};

#endif
