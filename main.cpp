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
    std::cout << "Vec : ";
    for(auto &str : cmds)
        std::cout << str;
    std::cout << "END";
    pr.br();

    if(levenshtein(cmds[0],  "write") < LEV_THRESHOLD)
    {
        _mark.write();
    }
    else if(levenshtein(cmds[0], "diagnose") < LEV_THRESHOLD)
    {
        std::string ret = "Average number of connections per node in this chain : " + std::to_string(_mark.getAverageNumConnections()) + '\n';
        pr.message( ret, BLUE );
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

    std::cin.clear();
}
