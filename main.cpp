#include <iostream>
#include <time.h>
#include <random>

#include "markovChain.hpp"
#include "notes.hpp"
#include "printer.hpp"
#include "sim_time.hpp"
#include "util.hpp"

#define LEV_THRESHOLD 2

void processInput(const std::string &_input, markovChain &_mark);

void visualise( markovChain &_mark );

int main(int argc, char* argv[])
{
	std::cout << "Oh heck!\n";

	loadConfig();
	g_PARAM_NOTESET_SIMILARITY_TOLERANCE = gint("noteset_similarity_tolerance_build");

	markovChain mark (2);
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
	else if(levenshtein(cmds[0], "add") < LEV_THRESHOLD)
	{
		if(cmds.size() > 1)
			_mark.loadSource( cmds[1] );
	}
	else if(levenshtein(cmds[0], "clear") < LEV_THRESHOLD)
	{
		_mark.clear();
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
				{
					_mark.stopSound();
					done = true;
				}
				else if(event.key.keysym.sym == SDLK_q)
					_mark.resetPos();
				else if(event.key.keysym.sym == SDLK_l)
				{
					_mark.toggleLight();
				}
				else if(event.key.keysym.sym == SDLK_o)
				{
					_mark.getVis()->addFOV( 1.0f );
				}
				else if(event.key.keysym.sym == SDLK_p)
				{
					_mark.getVis()->addFOV( -1.0f );
				}
				else if(event.key.keysym.sym == SDLK_EQUALS)
				{
					std::cout << "Timescale : " << g_TIME_SCALE << '\n';
					g_TIME_SCALE += 0.1f;
				}
				else if(event.key.keysym.sym == SDLK_MINUS)
				{
					std::cout << "Timescale : " << g_TIME_SCALE << '\n';
					g_TIME_SCALE = clamp( g_TIME_SCALE - 0.1f, 0.0f, F_MAX );
				}
				else if(event.key.keysym.sym == SDLK_c)
					_mark.toggleCameraLock();
				else if(event.key.keysym.sym == SDLK_s)
					_mark.getVis()->toggleSteadicam();
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

		g_TIME += timer.getDiff() * g_TIME_SCALE;

		timer.setCur();

        timer.clampAcc(0.05);

        sim_time d (0.0f);

        d.setStart();

		//Update the game in small time-steps (dependant on the timers fps).
		while(timer.getAcc() > timer.getFrame())
		{
			float frame = timer.getFrame() * g_TIME_SCALE;
			_mark.update( frame );
			timer.incrAcc( -frame );
		}

        d.setCur();
        std::cout << "Update time : " << d.getDiff() << " seconds\n";

		_mark.visualise( );
        d.setCur();
        std::cout << "Draw time : " << d.getDiff() << " seconds\n";
	}
	_mark.hideVisualiser();
}
