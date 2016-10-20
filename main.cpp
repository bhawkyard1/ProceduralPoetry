#include <iostream>
#include <time.h>
#include "markovChain.hpp"
#include "printer.hpp"
#include "util.hpp"

#define LEV_THRESHOLD 2

void processInput(const std::string &_input, markovChain &_mark);

int main(void)
{
    std::cout << "Oh heck!\n";

    srand(time(NULL));

    markovChain mark (3);
    printer pr;

    bool done = false;
    while(!done)
    {
        pr.message( "Enter a command : ", GREEN );

        std::string input;
        std::getline( std::cin, input );

        //std::cin >> input;

        processInput( input, mark );
    }

    return 0;
}

void processInput(const std::string &_input, markovChain &_mark)
{
    printer pr;
    std::vector<std::string> cmds = split( _input, ' ' );

    if(levenshtein(cmds[0],  "write") < LEV_THRESHOLD)
    {
        size_t wordCount = randNum(100, 200);
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
    else if(levenshtein(cmds[0], "quit") < LEV_THRESHOLD)
    {
        exit( EXIT_SUCCESS );
    }

    std::cin.clear();
}
