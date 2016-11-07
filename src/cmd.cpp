#include "cmd.hpp"
#include "util.hpp"

cmd::cmd(markovChain *_obj)
{
    m_obj = _obj;
}

void cmd::exec(const std::string &_cmd)
{
    std::vector<std::string> cmds = split(_cmd, ' ');
    std::string func = cmds[0];
    std::vector<std::string> args = cmds;
    args.erase( args.begin() );

    m_obj->(*m_commands[func])();
}

void cmd::reg(const std::string &_name, fpt _pt)
{
    std::pair<std::string, fpt> p(_name, _pt);
    m_commands.insert(p);
}
