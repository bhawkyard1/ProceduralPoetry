#ifndef CMD_HPP
#define CMD_HPP

#include <unordered_map>

#include "markovChain.hpp"

typedef void (* fpt)(void);

class cmd
{
public:
    cmd( markovChain * _obj );
    void reg(const std::string &_name, fpt _pt);
    void exec(const std::string &_cmd);
private:
    markovChain * m_obj;
    std::unordered_map<std::string, fpt> m_commands;
};

#endif
