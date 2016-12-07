#include <iostream>
#include <time.h>
#include <random>

#include "physicsvars.hpp"
#include "markovChain.hpp"
#include "notes.hpp"
#include "printer.hpp"
#include "sim_time.hpp"
#include "util.hpp"

#define LEV_THRESHOLD 2

template<class T>
void processInput(const std::string &_input, markovChain<T> &_mark);

template<class T>
void visualise( markovChain<T> &_mark );

int main(int argc, char* argv[])
{
    std::cout << "Oh heck!\n";

    markovChain<notes> mark (1);
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

template<class T>
void processInput(const std::string &_input, markovChain<T> &_mark)
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
    else if(levenshtein(cmds[0], "clearsources") < LEV_THRESHOLD)
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

template<class T>
void visualise(markovChain<T> &_mark)
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

        timer.clampAcc(0.2);

        //Update the game in small time-steps (dependant on the timers fps).
        while(timer.getAcc() > timer.getFrame())
        {
            float frame = timer.getFrame() * 16.0f * g_TIME_SCALE;
            _mark.update( frame );
            timer.incrAcc( -frame );
        }

        _mark.visualise( );
    }
    _mark.hideVisualiser();
}
