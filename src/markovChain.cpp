#include <iostream>

#include "markovChain.hpp"
#include "printer.hpp"
#include "file.hpp"
#include "util.hpp"

#ifdef _WIN32
#include <ciso646>
#endif

markovChain::markovChain(size_t _order)
{
    m_order = _order;

    resetBuffers();

    load();
}

void markovChain::reload(int _order)
{
    printer pr;
    m_order = _order;
    pr.message( "New order is " + std::to_string(m_order));
    resetBuffers();
    m_states.clear();
    pr.message("States cleared, size " + std::to_string(m_states.size()));

    load();
}

void markovChain::load()
{
    printer pr;

    pr.message( "Loading sources...\n", BRIGHTGREEN );
    std::vector<std::string> sources = getSources();
    pr.message( "Generating chain...\n" );
    for(auto &source : sources)
        loadSource(source);

    pr.message( "Chain size : " + std::to_string(m_states.size()) + '\n' );
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

void markovChain::diagnoseNode( const std::string &_str )
{
    std::vector<std::string> context = split(_str, ',');
    /*std::cout << "Split is ";
    for(auto &i : context)
        std::cout << i << ' ';
    std::cout << '\n';*/

    if(context.size() != m_order)
    {
        std::cerr << "Warning! Size of context is " << context.size() << ", which does not match the chain order, which is " << m_order << '\n';
        return;
    }

    if(m_states.count( context ) == 0)
    {
        std::cerr << "Warning! No node with this context!\n";
        return;
    }

    std::cout << "Diagnosing node '";
    for(auto &i : context) std::cout << ' ' << i;
    std::cout << "'\n";

    std::cout << "Connections : " << m_states[ context ].getNumConnections() << "\n     To nodes :\n";
    for(auto &i : m_states[ context ].getConnections())
    {
        std::string node = i.m_node;
        if(node == "\n")
            node = "\\n";
        std::cout << "      " << node << " P:" << (i.m_probability / m_states[context].getMaxProb()) << "\n";
    }
    std::cout << '\n';
}

float markovChain::getAverageNumConnections()
{
    size_t cons = 0;
    for(auto &i : m_states)
        cons += i.second.getNumConnections();
    cons = (float)cons / (float)m_states.size();
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
    std::cout << "Reading from " << _path << '\n';
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
    }
}

void markovChain::write(size_t _wordCount)
{
    printer pr;
    resetBuffers();
    pr.message("Writing...", RED);

    //Word counting variables
    size_t curWord = 0;

    //Grab a random key from m_states
    auto it = m_states.begin();
    std::advance( it, randNum((size_t)0, m_states.size()) );
    std::vector<std::string> curKey = it->first;
    //Add each string in the initial key to the context
    for(auto &str : curKey)
        addContext(str);

    pr.message("Initial key is ", CYAN);
    for(auto &i : curKey)pr.message( i + ' ' );
    pr.br();

    //Terminate when we have exceeded the word count and the current word is a line break.
    while(curWord < _wordCount or m_seekBuffer[0] != "\n")
    {
        pr.message( "Iteration  " + curWord + '\n', WHITE );
        pr.message( "Key " );
        for(auto &str : curKey) pr.message( str + ' ' );
        pr.br();

        //Grab outbound connection
        std::string next = m_states[ curKey ].getRandomConnection().m_node;

        pr.message( "Next is " + next + '\n' );

        if(next == "")
            break;

        //Write to buffer
        if(next != "\n")
            m_writeBuffer += next + ' ';
        else
            m_writeBuffer += next;

        //Add to context
        addContext( next );

        //Set cur key to reflect new context
        curKey = getKeyFromContext();

        curWord++;
    }
    pr.message( "\n\n" + m_writeBuffer + "\n\n", BRIGHTWHITE );
}
