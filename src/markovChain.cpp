#include <algorithm>
#include <iostream>
#include <ngl/Random.h>

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
/*
void markovChain::constructVisualisation()
{
    m_visualiser.clearPoints();

    ngl::Random * rnd = ngl::Random::instance();
    std::cout << "visualising\n";
    unsigned long cnt = 0;

    ngl::Vec3 origin = ngl::Vec3(0.0f, 0.0f, 0.0f);

    //A vector of all the keys that we have visited, we use this to keep track of nodes we have visited.
    std::vector< mKey > visitedKeys;
    //Connected keys to explore (will be checked agains visitedKeys PRIOR to adding).
    std::vector< mKey > exploreKeys;
    //Push back a random key to start on.
    exploreKeys.push_back( getRandomContext() );

    //Clear the seekbuffer, then construct a new context.
    m_seekBuffer.clear();
    for(auto &i : exploreKeys[0])
        addContext( i );

    //While there are keys to explore... keep exploring.
    while( exploreKeys.size() != 0 )
    {
        //The keys that, based on the exploration or explorekeys, will be added at the next iteration of the while loop.
        //This is just to avoid adding things straight to explorekeys, which I expect may cause problems, make the vector resize etc.
        std::vector< mKey > newKeys;
        for( auto &key : exploreKeys )
        {
            cnt++;
            //Connections to the currently explored node.
            std::vector< markovEdge > connections = m_states[ key ].getConnections();
            for( auto &edge : connections )
            {
                //Create a key to consider
                mKey testKey = key;
                //Form context using current edge.
                testKey.erase( testKey.begin() );
                testKey.push_back( edge.m_node );

                //If no node is associated with this key, continue
                //This shouldn't happen, I don't think...
                if( m_states.count( testKey ) == 0 )
                    continue;

                //Add testKey to newKeys
                newKeys.push_back( testKey );
            }
            std::string nodeName = toString(key);

            //Add point with offset.
            origin += rnd->getRandomNormalizedVec3();
            m_visualiser.addPoint( origin, nodeName );

            visitedKeys.push_back( key );
        }

        exploreKeys.clear();

        for( auto &newKey : newKeys )
        {
            if( std::find(
                        visitedKeys.begin(),
                        visitedKeys.end(),
                        newKey)
                    ==
                    visitedKeys.end()
                    )
            {
                exploreKeys.push_back( newKey );
            }
        }
    }
    std::cout << "Total of " << cnt << " keys explored\n";

    m_visualiser.show();
}
*/

void markovChain::constructVisualisation()
{
    printer pr;

    pr.message("Generating visualisation...\n");

    ngl::Random * rnd = ngl::Random::instance();

    //We need to keep track of connections that a node has.
    //Indexes match up with the indexes of the nodes being stored in the visualiser.
    //Each node/index may have multiple connections.
    std::vector< std::vector<mKey> > connections;

    pr.message("Adding points...\n");

    size_t index = 0;

    //Add points
    for(auto &state : m_states)
    {
        connections.push_back( {} );

        ngl::Vec3 pt = rnd->getRandomNormalizedVec3() * randFlt(0.0f, 256.0f);
        std::string name = toString(state.first);

        m_visualiser.addPoint( pt, name, state.second.getNumConnections() );

        //Add connections
        //Loop through state edges
        for(auto &i : state.second.getConnections())
        {
            //This initially points to our current state
            mKey connection = state.first;
            fifoQueue(&connection, i.m_node, m_order);

            connections[index].push_back(connection);
        }

        index++;
    }

    pr.message("Transcribing connections");

    //Do connections.
    slotmap<sphere> * vnodes = m_visualiser.getNodesPt();
    for(size_t i = 0; i < connections.size(); ++i)
    {
        if(i % (connections.size() / 10) == 0)
            pr.message(".");

        auto keys = connections[i];
        //Track down each connection and assign it to the visualiser node.
        for(auto &key : keys)
        {
            for(size_t j = 0; j < vnodes->size(); ++j)
            {
                //If key at this index matches the node name, connect nodes[i] to nodes[j]
                if(vnodes->get( j ).getName() == toString(key))
                {
                    slotID ref = vnodes->getID( j );
                    vnodes->get(i).addConnection(ref);
                    break;
                }
            }
        }
    }

    pr.br();

    //Do shitty gravity sim.
    pr.message("Arranging nodes");
    for(int it = 0; it < 1; ++it)
    {
        if(it % 10 == 0)
            pr.message(".");

        //For every point...
        /*for(size_t i = 0; i < vnodes->m_points.size(); ++i)
        {
            ngl::Vec3 origin = vnodes->m_points[i];
            //Loop through connection.
            for(auto &id : vnodes->m_connections[i])
            {
                ngl::Vec3 * target = vnodes->m_points.getByID( id );
                //Get distance.
                ngl::Vec3 dir = *target - origin;
                float dist = dir.length();
                dir /= dist * dist;

                //Add forces to both current node and connection node (connections are one-way so we must do both here).
                //vnodes->addVel( i, dir );
                //vnodes->addVel( vnodes->m_velocities.getIndex( id ), -dir );
            }
        }

        vnodes->integrateVelocity();*/
    }

    m_visualiser.show();
}

/*void markovChain::constructVisualisation()
{
    printer pr;

    pr.message("Generating visualisation...\n");

    ngl::Random * rnd = ngl::Random::instance();

    //We need to keep track of connections that a node has.
    //Indexes match up with the indexes of the nodes being stored in the visualiser.
    //Each node/index may have multiple connections.
    std::vector< std::vector<mKey> > connections;

    pr.message("Adding points...\n");

    size_t index = 0;

    //Add points
    for(auto &state : m_states)
    {
        connections.push_back( {} );

        ngl::Vec3 pt = rnd->getRandomNormalizedVec3() * randFlt(0.0f, 256.0f);
        std::string name = toString(state.first);

        m_visualiser.addPoint( pt, name );

        //Add connections
        //Loop through state edges
        for(auto &i : state.second.getConnections())
        {
            //This initially points to our current state
            mKey connection = state.first;
            fifoQueue(&connection, i.m_node, m_order);

            connections[index].push_back(connection);
        }

        index++;
    }

    pr.message("Transcribing connections");

    //Do connections.
    visualiserNodes * vnodes = m_visualiser.getNodesPt();
    for(size_t i = 0; i < connections.size(); ++i)
    {
        if(i % (connections.size() / 10) == 0)
            pr.message(".");

        auto keys = connections[i];
        //Track down each connection and assign it to the visualiser node.
        for(auto &key : keys)
        {
            for(size_t j = 0; j < vnodes->m_points.size(); ++j)
            {
                //If key at this index matches the node name, connect nodes[i] to nodes[j]
                if(vnodes->m_strings[j] == toString(key))
                {
                    slotID ref = vnodes->m_connections.getID( j );
                    vnodes->m_connections[i].push_back(ref);
                    break;
                }
            }
        }
    }

    pr.br();

    //Do shitty gravity sim.
    pr.message("Arranging nodes");
    for(int it = 0; it < 1; ++it)
    {
        if(it % 10 == 0)
            pr.message(".");

        //For every point...
        for(size_t i = 0; i < vnodes->m_points.size(); ++i)
        {
            ngl::Vec3 origin = vnodes->m_points[i];
            //Loop through connection.
            for(auto &id : vnodes->m_connections[i])
            {
                ngl::Vec3 * target = vnodes->m_points.getByID( id );
                //Get distance.
                ngl::Vec3 dir = *target - origin;
                float dist = dir.length();
                dir /= dist * dist;

                //Add forces to both current node and connection node (connections are one-way so we must do both here).
                //vnodes->addVel( i, dir );
                //vnodes->addVel( vnodes->m_velocities.getIndex( id ), -dir );
            }
        }

        vnodes->integrateVelocity();
    }

    m_visualiser.show();
}*/

void markovChain::update(const float _dt)
{
    m_visualiser.update(_dt);
}

void markovChain::visualise()
{
    m_visualiser.drawSpheres();
    m_visualiser.finalise();
}

/*
void markovChain::constructVisualisation()
{
    std::cout << "visualising\n";

    //Get random starting key.
    std::vector<std::string> start = getRandomContext();

    std::cout << "p1\n";

    //Clear the seekbuffer, then construct a new context.
    m_seekBuffer.clear();
    for(auto &i : start)
        addContext( i );

    std::cout << "p2\n";

    ngl::Vec3 origin = ngl::Vec3(0.0f, 0.0f, 0.0f);

    std::cout << "starting recursion\n";
    recursiveNodeData(
                m_states[ start ],
                origin,
                m_seekBuffer
                );
}
*/
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
    /*m_seekBuffer.push_back(_str);
    if(m_seekBuffer.size() > m_order)
        m_seekBuffer.erase( m_seekBuffer.begin() );*/

    fifoQueue(
                &m_seekBuffer,
                _str,
                m_order
                );
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

    printer pr;
    std::string str;

    str += "Diagnosing node '";
    for(auto &i : context) str += ' ' + i;
    str += "'\n";

    pr.message( str, BRIGHTGREEN );

    pr.message( "Connections : " + std::to_string(m_states[ context ].getNumConnections()) + "\n     To nodes :\n", BRIGHTCYAN );
    for(auto &i : m_states[ context ].getConnections())
    {
        std::string node = i.m_node;
        if(node == "\n")
            node = "\\n";
        pr.message( "      " + node + " P : " + std::to_string( i.m_probability / m_states[ context ].getMaxProb() ) + "\n" );
    }
    pr.br();
}

float markovChain::getAverageNumConnections()
{
    size_t cons = 0;
    for(auto &i : m_states)
        cons += i.second.getNumConnections();
    cons = (float)cons / (float)m_states.size();
    return cons;
}

mKey markovChain::getKeyFromContext()
{
    std::vector<std::string> ret;

    for(auto &str : m_seekBuffer)
        ret.push_back(str);

    return ret;
}

mKey markovChain::getRandomContext()
{
    //Grab a random key from m_states
    auto it = m_states.begin();
    int advance = randInt(0, m_states.size());
    std::advance( it, advance );

    std::vector<std::string> key = it->first;
    std::cout << "Context is ";
    for(auto &i : key)
        std::cout << " " << i;
    std::cout << '\n';

    return key;
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

    std::vector<std::string> curKey = getRandomContext();

    //Add each string in the initial key to the context
    for(auto &str : curKey)
        addContext(str);

    //Terminate when we have exceeded the word count and the current word is a line break.
    while(curWord < _wordCount or m_seekBuffer[0] != "\n")
    {
        /*pr.message( "Iteration  " + curWord + '\n', WHITE );
        pr.message( "Key " );
        for(auto &str : curKey) pr.message( str + ' ' );
        pr.br();*/

        //Grab outbound connection
        std::string next = m_states[ curKey ].getRandomConnection().m_node;

        if(next == "")
        {
            std::string key;
            for(auto &i : m_seekBuffer)
            {
                if(i == "\n")
                    key += "\\n ";
                else
                    key += i + ' ';
            }
            pr.message( key + "has zero connecting nodes :(" );
            break;
        }
        //pr.message( "Next is " + next + '\n' );

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
