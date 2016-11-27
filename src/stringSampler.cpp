#include "stringSampler.hpp"
#include "file.hpp"

void stringSampler::sample(const std::string &_source, markovChain<std::string> * _chain)
{
    std::cout << "Reading from " << _source << '\n';
    std::vector<std::string> inputs = loadMarkovSource(_source);

    for(size_t i = 0; i < inputs.size(); ++i)
    {
        //Add our input to the queue
        _chain->addContext(inputs[i]);

        //If we have accumulated enough of a context
        if(m_seekBuffer.size() == m_order)
        {
            std::vector<std::string> key = getKeyFromContext();

            //If a node defined by this context does not exist, add.
            if(m_states.count( key ) == 0)
            {
                //std::cout << "Adding node " << inputs[i] << '\n';
                std::pair<std::vector<std::string>, markovState<T>> entry (key, markovState<T>());
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
