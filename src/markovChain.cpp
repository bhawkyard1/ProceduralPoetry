#include <iostream>

#include "markovChain.hpp"
#include "file.hpp"
#include "util.hpp"

#ifdef _WIN32
#include <ciso646>
#endif

markovChain::markovChain(size_t _order)
{
    m_order = _order;

    std::cout << "Loading sources...\n";
    std::vector<std::string> sources = getSources();
    std::cout << "Generating chain...\n";
    for(auto &source : sources)
        loadSource(source);
    std::cout << "Chain size : " << m_states.size() << '\n';
}

void markovChain::addNode(const markovState &_node)
{
    //m_states;
}

void markovChain::loadSource(const std::string _path)
{
    //std::cout << "Reading from " << _path << '\n';
    std::vector<std::string> inputs = loadMarkovSource(_path);

    for(size_t i = 0; i < inputs.size(); ++i)
    {
        //Add node if none exists
        if(m_states.count(inputs[i]) == 0)
        {
            //std::cout << "Adding node " << inputs[i] << '\n';
            std::pair<std::string, markovState> entry (inputs[i], markovState());
            m_states.insert(
                        entry
                        );
        }

        //Connect
        for(size_t j = i + 1; j < i + m_order + 1 and j < inputs.size(); ++j)
        {
            //std::cout << "Connecting node " << inputs[i] << " to " << inputs[j] << '\n';
            m_states[inputs[i]].addConnection(inputs[j]);
        }
    }
}

void markovChain::write()
{
    std::cout << "Writing...";
    size_t wordCount = randNum(100, 200);
    size_t curWord = 0;
    std::string curString = "\n";

    //Terminate when we have exceeded the word count and the current word is a line break.
    while(curWord < wordCount or curString != "\n")
    {
        std::string next = m_states[curString].getRandomConnection().m_node;
        std::cout << next << " ";
        curString = next;
        curWord++;
    }
}
