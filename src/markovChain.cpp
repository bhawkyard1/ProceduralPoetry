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

    resetBuffers();

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

void markovChain::addContext(const std::string &_str)
{
    m_seekBuffer.push_back(_str);
    if(m_seekBuffer.size() > m_order)
        m_seekBuffer.pop_front();
}

float markovChain::getAverageNumConnections()
{
    size_t cons = 0;
    for(auto &i : m_states)
        cons += i.second.getNumConnections();
    cons /= (float)m_states.size();
    return cons;
}

std::vector<std::string> markovChain::getKeyFromContext()
{
    std::vector<std::string> ret;

    for(auto &str : m_seekBuffer)
        ret.push_back(str);

    return ret;
}

void markovChain::loadSource(const std::string _path)
{
    resetBuffers();
    //std::cout << "Reading from " << _path << '\n';
    std::vector<std::string> inputs = loadMarkovSource(_path);

    for(size_t i = 0; i < inputs.size(); ++i)
    {
        //Add our input to the queue
        addContext(inputs[i]);

        //If we have accumulated enough of a context
        if(m_seekBuffer.size() == m_order)
        {
            std::vector<std::string> key = getKeyFromContext();

            //If a node defined by this context does not exist, add.
            if(m_states.count( key ) == 0)
            {
                //std::cout << "Adding node " << inputs[i] << '\n';
                std::pair<std::vector<std::string>, markovState> entry (key, markovState());
                m_states.insert(
                            entry
                            );
            }

            //If accessing the NEXT string will not be out of bounds...
            if( i + 1 < inputs.size() )
            {
                //Connect the current context
                m_states[ key ].addConnection( inputs[i + 1] );
            }
        }

        //Add node if none exists
        /*if(m_states.count(inputs[i]) == 0)
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
        }*/
    }
}

void markovChain::write()
{
    resetBuffers();
    std::cout << "Writing...";

    //Word counting variables
    size_t wordCount = randNum(100, 200);
    size_t curWord = 0;

    //Grab a random key from m_states
    auto it = m_states.begin();
    std::advance( it, randNum((size_t)0, m_states.size()) );
    std::vector<std::string> curKey = it->first;
    //Add each string in the initial key to the context
    for(auto &str : curKey)
        addContext(str);

    std::cout << "Initial key is ";
    for(auto &i : curKey) std::cout << i << ' ';
    std::cout << '\n';

    //Terminate when we have exceeded the word count and the current word is a line break.
    while(curWord < wordCount or m_seekBuffer[0] != "\n")
    {
        std::cout << "Iteration " << curWord << '\n';
        std::cout << "Key ";
        for(auto &str : curKey) std::cout << str << ' ';
        std::cout << '\n';

        //Grab outbound connection
        std::string next = m_states[ curKey ].getRandomConnection().m_node;

        std::cout << "Next is " << next << '\n';

        if(next == "")
            break;

        //Write to buffer
        m_writeBuffer += next + ' ';

        //Add to context
        addContext( next );

        //Set cur key to reflect new context
        curKey = getKeyFromContext();

        curWord++;
    }
    std::cout << m_writeBuffer;
}
