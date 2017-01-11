#ifndef MARKOVCHAIN_HPP
#define MARKOVCHAIN_HPP

#include <algorithm>
#include <iostream>
#include <ngl/Random.h>

#include <SDL2/SDL_mixer.h>

#include <map>
#include <queue>
#include <string>

#include "common.hpp"

#include "markovState.hpp"
#include "visualiser.hpp"

#include "notes.hpp"

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

	visualiser * getVis() {return &m_visualiser;}

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

	void toggleCameraLock() {m_visualiser.toggleCameraLock();}
	void toggleLight() {m_visualiser.toggleLight();}
	void stopSound() {m_visualiser.stopSound();}
private:
	void addContext(const T &_state);
	std::vector<T> getKeyFromContext();
	void resetBuffers() {m_writeBuffer = ""; m_seekBuffer.clear();}
	void load();

	size_t m_order;

	//We write into this buffer, and can write to disk/display to screen.
	std::string m_writeBuffer;

	//Traversing m_states will only return one string per step. This is written into a buffer (FIFO queue) to give context for the next step.
	std::vector<T> m_seekBuffer;

	//This is the guts of the chain, the vector of T (which will be order in length) is the key, and a markov state takes the value.
	std::map< std::vector<T>, markovState<T>, notesComp > m_states;

	visualiser m_visualiser;
};

template<class T>
markovChain<T>::markovChain(size_t _order) :
	m_visualiser(_order)
{
	std::cout << "Markov chain ctor!\n";
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
	printer pr;

	pr.message("Generating visualisation...\n");

	ngl::Random * rnd = ngl::Random::instance();

	//We need to keep track of connections that a node has.
	//Indexes match up with the indexes of the nodes being stored in the visualiser.
	//Each node/index may have multiple connections.
	//Remember, note is a note, std::vector note is a state.
	//For each state->store a bunch of->connecty bits representing connections.
	std::vector<            std::vector<                notes > > connections;

	pr.message("Adding points...\n");

	size_t index = 0;
	//Add points
	for(auto &state : m_states)
	{
		//Add a (currently empty) list of connections from THIS state.
		connections.push_back( {} );

		//Compute node constructor variables
		ngl::Vec3 pt = rnd->getRandomNormalizedVec3() * randFlt(0.0f, 256.0f);
		std::vector< notes > name = state.first;
		float mass = state.second.getNumConnections();
		mass = sqrtf(mass);
		//mass = clamp((mass), 0.0f, 10.0f);

		m_visualiser.addPoint( pt, name, mass );

		//Add connections
		//Loop through markov edges
		for(auto &connection : state.second.getConnections())
			connections[index].push_back( connection.m_node );
		index++;
	}

	pr.message("Transcribing connections");

	//Do connections.
	slotmap<sphere> * vnodes = m_visualiser.getNodesPt();
	for(size_t i = 0; i < connections.size(); ++i)
	{
		if(i % (connections.size() / 10) == 0)
			pr.message(".");

		for(auto &connection : connections[i])
		{
			std::vector< notes > vnodeName = (*vnodes)[i].getName();
			fifoQueue( &vnodeName, connection, m_order );

			for(size_t j = 0; j < vnodes->size(); ++j)
				if((*vnodes)[j].getName() == vnodeName)
					(*vnodes)[i].addConnection( vnodes->getID(j) );
		}
	}

	/*for(auto &i : vnodes->m_objects)
				{
								std::cout << "nocon " << i.getConnections()->size() << ", " << vnodes->size() << ", " << connections.size() << ", " << m_states.size() << ", " << index << '\n';
								}*/

	pr.br();

	//Do shitty gravity sim.
	pr.message("Arranging nodes");
	for(int it = 0; it < 1; ++it)
	{
		if(it % 10 == 0)
			pr.message(".");
	}

    g_PARAM_NOTESET_SIMILARITY_TOLERANCE = gint("noteset_similarity_tolerance_runtime"); //;gint("noteset_similarity_tolerance_runtime")/*g_PARAM_NOTESET_SIMILARITY_TOLERANCE_RUNTIME*/;

	m_visualiser.show();
	m_visualiser.sound( g_RESOURCE_LOC + "poems/" + getSources()[0] );
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
void markovChain<T>::addContext(const T &_state)
{
	/*m_seekBuffer.push_back(_str);
																if(m_seekBuffer.size() > m_order)
																																m_seekBuffer.erase( m_seekBuffer.begin() );*/

	fifoQueue(
				&m_seekBuffer,
				_state,
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
	std::cout << "A\n";
	std::vector<T> ret;

	std::cout << "B " << m_seekBuffer.size() << '\n';
	for(auto &state : m_seekBuffer)
	{
		std::cout << "		loop\n";
		ret.push_back(state);
		std::cout << "		post push\n";
	}
	return ret;
}

template<class T>
std::vector<T> markovChain<T>::getRandomContext()
{
	//Grab a random key from m_states
	auto it = m_states.begin();
	int advance = randInt(0, m_states.size());
	std::advance( it, advance );

	std::vector<T> key = it->first;
	std::cout << "Context is ";
	for(auto &i : key)
	{
		for(auto &note : i)
			std::cout << " " << sNotes[note.m_type] + std::to_string(note.m_position);
		std::cout << '\n';
	}
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
	std::vector<T> states;
	while(!done)
	{
		//std::cout << "TIME : " << time;
		//Get raw fft
        std::vector<float> data = smpl.sampleAudio( time, gint("sample_width_bytes") );/*g_PARAM_SAMPLE_WIDTH*/

        done = gint("sample_width_bytes")/*g_PARAM_SAMPLE_WIDTH*/ + smpl.secsToBytes(time) >= smpl.get()->alen;
        time += gflt("sound_sample_frequency")/*g_PARAM_SAMPLE_TIMESTEP*/;

        /*int accWidth = gint("sample_width_bytes") / g_PARAM_AVERAGED_WIDTH;

								//Average values, condense into smaller array.
								std::vector<float> averaged;
								averageVector(data, averaged, accWidth);*/

		//Create the state, std::vector<note> being played.
		//std::vector<note> state;
		/*state.reserve( g_PARAM_AVERAGED_WIDTH );
								//Create nodes based on averages.
								for(size_t i = 0; i < averaged.size(); ++i)
								{
												//Min and max indexes to search for peak.
												int mindex = i - g_PARAM_PEAK_WIDTH / 2;
												int maxdex = i + g_PARAM_PEAK_WIDTH / 2;
												if(mindex < 0)
																maxdex += -mindex;
												if(maxdex > averaged.size())
																mindex += averaged.size() - maxdex;
												mindex = std::max(0, mindex);
												maxdex = std::min((int)averaged.size() , maxdex);

												//Get average around current index. Average of an average, I know. This probably isn't very nice to read.
												float averagedAverage = 0.0f;
												for(size_t j = mindex; j < maxdex; ++j)
												{
																averagedAverage += averaged[j];gold
												}
												averagedAverage /= maxdex - mindex;

												//If this is a non-duplicate peak, add a note to the state.
												if(averaged[i] > averagedAverage * g_PARAM_ACTIVATE_THRESHOLD_MUL)
												{
																float freq = i * sampler::getSampleRate() / (averaged.size() * accWidth);
																note closest = closestNote(freq);

																if(g_PARAM_USE_OCTAVES)
																{
                                                                                if(closest.m_position > gint("min_octave")/*g_PARAM_OCTAVE_MIN_CLIP and closest.m_position < gint("max_octave")g_PARAM_OCTAVE_MAX_CLIP and std::find(state.begin(), state.end(), closest) == state.end())
																				{
																								//std::cout << "Adding note " << closest.m_type << " " << closest.m_position << '\n';
																								state.push_back( closest );
																				}
																}
																else
																{
																				closest.m_position = 0;
																				if(std::find(state.begin(), state.end(), closest) == state.end())
																				{
																								//std::cout << "Adding note " << closest.m_type << " " << closest.m_position << '\n';
																								state.push_back( closest );
																				}
																}
												}
								}*/

		std::vector<float> ni = getNoteVals( data );

		for(size_t i = 0; i < ni.size(); ++i)
			fifoQueue( &g_noteIntensity[i], ni[i], g_noteIntensityOrder );
		for(size_t i = 0; i < g_noteIntensity.size(); ++i)
			g_averageNoteIntensity[i] = std::accumulate(g_noteIntensity[i].begin(), g_noteIntensity[i].end(), 0.0f) / g_noteIntensity[i].size();

		/*for(auto &i : ni)
						std::cout << "Val " << i << '\n';*/
		std::vector<note> activeNotes = getActiveNotes(ni);
		//std::cout << "activeNotes size " << activeNotes.size() << '\n';
		if(activeNotes.size() > 0)
			states.push_back( activeNotes );
	}
	for(size_t i = 0; i < g_averageNoteIntensity.size(); ++i)
		std::cout << sNotes[i % 12] << i / 12 << " : " << g_averageNoteIntensity[i] << '\n';
	std::cout << "Done accumulating states! There are " << states.size() << " of them!\n";

	m_states.clear();
	m_seekBuffer.clear();
	for(size_t i = 0; i < states.size(); ++i)
	{
		//std::cout << i << " of " << states.size() << '\n';
		addContext(states[i]);

		//If we have accumulated enough of a context
		if(m_seekBuffer.size() == m_order)
		{
			if(i + 1 < states.size())
			{
				/*m_states.insert(std::make_pair(m_seekBuffer, markovState<T>() ));
																m_states.at( m_seekBuffer ).addConnection( states[i + 1] );*/
				m_states[m_seekBuffer].addConnection( states[i + 1] );
			}
		}
	}
	std::cout << "m_states size is " << m_states.size() << '\n';

	/* for(auto &state : m_states)
				{
								std::cout << "state " << &state << " has : " << state.second.getNumConnections() << '\n';
				}
				std::cout << "Loaded, m_states.size() = " << m_states.size() << '\n';*/
}

template<class T>
void markovChain<T>::write(size_t _wordCount)
{
	printer pr;
	resetBuffers();
	pr.message("Writing...", RED);

	//Word counting variables
	size_t curWord = 0;

	std::vector<T> curKey = getRandomContext();

	//Add each string in the initial key to the context
	for(auto &state : curKey)
		addContext(state);

	//Terminate when we have exceeded the word count and the current word is a line break.
	while(curWord < _wordCount)
	{
		//Grab outbound connection
		std::vector<note> next = m_states.at( curKey ).getRandomConnection().m_node;

		if(next.size() == 0)
		{
			std::string key;
			for(auto &i : m_seekBuffer)
			{
				for(auto &note : i)
					key += sNotes[note.m_type] + " " + std::to_string(note.m_position) + "		";
				key += '\n';
			}
			pr.message( key + "has zero connecting nodes :(" );
			break;
		}
		//pr.message( "Next is " + next + '\n' );

		for(auto &note : next)
			m_writeBuffer += sNotes[note.m_type] + " " + std::to_string(note.m_position) + ", ";
		m_writeBuffer += '\n';

		//Add to context
		addContext( next );

		//Set cur key to reflect new context
		curKey = getKeyFromContext();

		curWord++;
	}
	pr.message( "\n\n" + m_writeBuffer + "\n\n", BRIGHTWHITE );
}

#endif
