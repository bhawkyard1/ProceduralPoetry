#include <iostream>
#include <time.h>
#include "markovChain.hpp"

void processInput(const std::string &_input, markovChain &_mark);

int main(void)
{
    std::cout << "Oh heck!\n";

    srand(time(NULL));

    markovChain mark (2);

    bool done = false;
    while(!done)
    {
        std::cout << "Enter a command ";

        std::string input;
        std::cin >> input;

        processInput( input, mark );
    }

    return 0;
}

void processInput(const std::string &_input, markovChain &_mark)
{
    if(_input == "write")
    {
        _mark.write();
    }
    else if(_input == "diagnose")
    {
        std::cout << "Average number of connections per node in this chain : " << _mark.getAverageNumConnections() << '\n';
    }

    std::cin.clear();
}
