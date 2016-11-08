#ifndef CMD_HPP
#define CMD_HPP

#include <unordered_map>

#include "markovChain.hpp"

template<class T>
class cmd
{
public:	
		typedef void (T::*fpt)(const std::string&);

		cmd( T * _obj )
		{
			m_object = _obj;
		}

		void reg(const std::string &_name, fpt _pt)
		{
			std::pair<std::string, fpt> p(_name, _pt);
			m_commands.insert(p);
		}

		void exec(const std::string &_cmd)
		{
			(m_obj->m_commands[_cmd])();
		}

private:
		T * m_obj;
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
