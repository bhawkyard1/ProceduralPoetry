#ifndef MARKOVCHAIN_HPP
#define MARKOVCHAIN_HPP

#include <algorithm>
#include <iostream>
#include <ngl/Random.h>

#include <SDL2/SDL_mixer.h>

#include <map>
#include <queue>
#include <string>

#include "markovState.hpp"
#include "range.hpp"
#include "visualiser.hpp"

#include "printer.hpp"
#include "file.hpp"
#include "util.hpp"

#include "sim_time.hpp"
#include "sampler.hpp"

#ifdef _WIN32
#include <ciso646>
#endif

template<class T>
class markovChain
{
	friend class stringSampler;
public:
	markovChain(size_t _order);

	void addNode(const markovState<T> &_node);

	void clear();

	void diagnoseNode( const std::string &_str );

	float getAverageNumConnections();

	std::vector<T> getRandomContext();

	void hideVisualiser() {m_visualiser.hide();}

	void loadSource( const std::string _path );

	void mouseDown(SDL_Event _event) {m_visualiser.mouseDown(_event);}
	void mouseUp(SDL_Event _event) {m_visualiser.mouseUp(_event);}
	void mouseWheel(int _dir) {m_visualiser.mouseWheel(_dir);}

	void write(size_t _wordCount);

	void reload(int _order);

	void showVisualiser() {m_visualiser.show();}

	size_t getOrder() {return m_order;}

	void constructVisualisation();

	void update(const float _dt);
	void visualise();

	void resetPos() {m_visualiser.resetPos();}

	void toggleLight() {m_visualiser.toggleLight();}

private:
	void addContext(const std::string &_str);
	std::vector<T> getKeyFromContext();
	void resetBuffers() {m_writeBuffer = ""; m_seekBuffer.clear();}
	void load();

	size_t m_order;

	//We write into this buffer, and can write to disk/display to screen.
	std::string m_writeBuffer;

	//Traversing m_states will only return one string per step. This is written into a buffer (FIFO queue) to give context for the next step.
	std::vector<T> m_seekBuffer;

	//This is the guts of the chain, the vector of strings (which will be order in length) is the string associates
	std::map< std::vector<T>, markovState<T> > m_states;

	visualiser m_visualiser;
};

template<class T>
markovChain<T>::markovChain(size_t _order)
{
	m_order = _order;

	resetBuffers();

	load();
}

template<class T>
void markovChain<T>::clear()
{
	m_writeBuffer = "";
	m_seekBuffer.clear();
	m_states.clear();

	m_visualiser.clearPoints();
}

template<class T>
void markovChain<T>::constructVisualisation()
{
	return;
	/*printer pr;

	pr.message("Generating visualisation...\n");

	ngl::Random * rnd = ngl::Random::instance();

	//We need to keep track of connections that a node has.
	//Indexes match up with the indexes of the nodes being stored in the visualiser.
	//Each node/index may have multiple connections.
	std::vector< std::vector<std::vector<T>> > connections;

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
			std::vector<T> connection = state.first;
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
					slot ref = vnodes->getID( j );
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
	}
	m_visualiser.show();*/
}


template<class T>
void markovChain<T>::update(const float _dt)
{
	m_visualiser.update(_dt);
}

template<class T>
void markovChain<T>::visualise()
{
	m_visualiser.drawSpheres();
	m_visualiser.finalise();
}


template<class T>
void markovChain<T>::reload(int _order)
{
	printer pr;
	m_order = _order;
	pr.message( "New order is " + std::to_string(m_order));
	resetBuffers();
	m_states.clear();
	pr.message("States cleared, size " + std::to_string(m_states.size()));

	load();
}

template<class T>
void markovChain<T>::load()
{
	printer pr;

	pr.message( "Loading sources...\n", BRIGHTGREEN );
	std::vector<std::string> sources = getSources();
	pr.message( "Generating chain...\n" );
	for(auto &source : sources)
		loadSource(source);

	pr.message( "Chain size : " + std::to_string(m_states.size()) + '\n' );
}

template<class T>
void markovChain<T>::addNode(const markovState<T> &_node)
{
	//m_states;
}

template<class T>
void markovChain<T>::addContext(const std::string &_str)
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

template<class T>
void markovChain<T>::diagnoseNode(const std::string &_str)
{
	return;
}
/*
template<class T>
void markovChain<T>::diagnoseNode( const std::string &_str )
{
		std::vector<std::string> context = split(_str, ',');

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
}*/

template<class T>
float markovChain<T>::getAverageNumConnections()
{
	size_t cons = 0;
	for(auto &i : m_states)
		cons += i.second.getNumConnections();
	cons = (float)cons / (float)m_states.size();
	return cons;
}

template<class T>
std::vector<T> markovChain<T>::getKeyFromContext()
{
	std::vector<std::string> ret;

	for(auto &str : m_seekBuffer)
		ret.push_back(str);

	return ret;
}

template<class T>
std::vector<T> markovChain<T>::getRandomContext()
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
/*
template<class T>
void markovChain<T>::loadSource(const std::string _path)
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
*/
template<class T>
void markovChain<T>::loadSource(const std::string _path)
{
	resetBuffers();
	std::cout << "Reading from " << _path << '\n';

	sampler::initialiseAudio( 44100, 2 );
	sampler smpl( g_RESOURCE_LOC + "poems/" + _path );

	float time = 0.0f;

	bool done = false;
	while(!done)
	{
		//Get raw fft
		std::vector<float> data = smpl.sampleAudio( time, 16384 );
		done = 8192 + smpl.secsToBytes(time) > smpl.get()->alen;
		time += 1.0f / 60.0f;
        /*std::cout << "TIME " << time << " DONE " << (8192 + smpl.secsToBytes(time)) << " vs " << smpl.get()->alen << '\n';

		for(auto &i : data)
            std::cout << i << '\n';*/

		//Average values
		std::vector<float> averaged;
		averaged.reserve( 4096 );
		for(size_t i = 0; i < averaged.size(); ++i)
			averaged[i] = std::accumulate(data.begin() + i, data.begin() + i + 4, 0);
		for(auto &i : averaged)
			i /= 4.0f;
		/*for(auto &i : averaged)
			if( i > 0.01f ) std::cout << "		" << i << '\n';*/
	}
	std::cout << "Done!\n";

	/*for(size_t i = 0; i < inputs.size(); ++i)
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
		}*/
}

template<class T>
void markovChain<T>::write(size_t _wordCount)
{
	return;
	/*printer pr;
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
	pr.message( "\n\n" + m_writeBuffer + "\n\n", BRIGHTWHITE );*/
}

#endif
