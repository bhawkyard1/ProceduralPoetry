#include <iostream>
#include <time.h>
#include "markovChain.hpp"

int main(void)
{
    std::cout << "Oh heck!\n";

    srand(time(NULL));

    markovChain mark (1);
    mark.write();

    return 0;
}
