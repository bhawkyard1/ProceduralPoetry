#include <iostream>
#include <time.h>
#include <random>

#include "markovChain.hpp"
#include "printer.hpp"
#include "sim_time.hpp"
#include "util.hpp"

#define LEV_THRESHOLD 2

void processInput(const std::string &_input, markovChain &_mark);
void visualise( markovChain &_mark );

int main(int argc, char* argv[])
{
	std::cout << "Oh heck!\n";

	markovChain mark (3);
	printer pr;

	bool done = false;
	while(!done)
	{
		pr.message( "Enter a command : ", GREEN );

		std::string input = "";
		std::getline( std::cin, input );

		//std::cin >> input;
		if(input.length() > 0)
			processInput(input, mark);
	}

	return 0;
}

void processInput(const std::string &_input, markovChain &_mark)
{
	printer pr;
	std::vector<std::string> cmds = split( _input, ' ' );

	if(levenshtein(cmds[0], "write") < LEV_THRESHOLD)
	{
		size_t wordCount = randInt(100, 200);
		if(cmds.size() > 1)
			wordCount = stoi(cmds[1]);
		_mark.write(wordCount);
	}
	else if(levenshtein(cmds[0], "diagnose") < LEV_THRESHOLD)
	{
		if(cmds.size() < 2)
		{
			std::string ret = "Average number of connections per node in this chain : " + std::to_string(_mark.getAverageNumConnections()) + '\n';
			pr.message( ret, BLUE );
		}
		else
		{
			_mark.diagnoseNode( cmds[1] );
		}
	}
	else if(levenshtein(cmds[0], "order") < LEV_THRESHOLD)
	{
		if(cmds.size() == 1)
			pr.message( "Order is " + std::to_string( _mark.getOrder() ) + '\n', YELLOW);
		else
		{
			pr.message( "Setting order.\n", YELLOW );
			_mark.reload( std::stoi( cmds[1] ) );
		}
	}
	else if(levenshtein(cmds[0], "v") < LEV_THRESHOLD)
	{
		_mark.constructVisualisation();
		visualise(_mark);
	}
	else if(levenshtein(cmds[0], "addsource") < LEV_THRESHOLD)
	{
		if(cmds.size() > 1)
			_mark.loadSource( cmds[1] );
	}
	else if(levenshtein(cmds[0], "quit") < LEV_THRESHOLD)
	{
		exit( EXIT_SUCCESS );
	}
	else
	{
		pr.message( "Sorry, I did not understand that instruction :(\n" );
	}

	std::cin.clear();
}

void visualise(markovChain &_mark)
{
	sim_time timer(120.0f);
	bool done = false;
	while(!done)
	{
		SDL_Event event;
		while(SDL_PollEvent( &event ))
		{
			switch( event.type )
			{
			case SDL_QUIT:
				return;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					done = true;
				else if(event.key.keysym.sym == SDLK_q)
					_mark.resetPos();
				else if(event.key.keysym.sym == SDLK_l)
				{
					_mark.toggleLight();
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				_mark.mouseDown( event );
				break;
			case SDL_MOUSEBUTTONUP:
				_mark.mouseUp( event );
				break;
			case SDL_MOUSEWHEEL:
				_mark.mouseWheel( event.wheel.y );
				break;
			default:
				break;
			}
		}

		g_TIME += timer.getDiff();

		timer.setCur();

		//Update the game in small time-steps (dependant on the timers fps).
		while(timer.getAcc() > timer.getFrame())
		{
			_mark.update( timer.getDiff() * 512.0f );
			timer.incrAcc( -timer.getDiff() );
		}

		_mark.visualise( );
	}
	_mark.hideVisualiser();
}
