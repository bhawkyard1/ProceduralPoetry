#ifndef CMD_HPP
#define CMD_HPP

#include <unordered_map>

#include "markovChain.hpp"

class cmd
{
public:	
    typedef void (*fpt)(const std::string&);
    cmd() = default;

    void reg(const std::string &_name, fpt _pt)
    {
        std::pair<std::string, fpt> p(_name, _pt);
        m_commands.insert(p);
    }

    void exec(const std::string &_cmd)
    {
        auto command = splitFirst( _cmd, ' ' );
        (*m_commands[command.first])(command.second);
    }

private:
    std::unordered_map<std::string, fpt> m_commands;
};

#endif
/*
cmd::cmd(T *_obj)
{
        m_obj = _obj;
}

void cmd::exec(const std::string &_cmd)
{
        std::vector<std::string> cmds = split(_cmd, ' ');
        std::string func = cmds[0];
        std::vector<std::string> args = cmds;
        args.erase( args.begin() );

        (m_obj->*m_commands[func])();
}

void cmd::reg(const std::string &_name, fpt _pt)
{
        std::pair<std::string, fpt> p(_name, _pt);
        m_commands.insert(p);
}
*/
