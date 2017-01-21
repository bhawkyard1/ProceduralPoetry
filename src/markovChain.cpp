#include "markovChain.hpp"

markovChain::markovChain(size_t _order) :
	m_visualiser(_order)
{
	std::cout << "Markov chain ctor!\n";
	m_order = _order;

	resetBuffers();

	load();
}


void markovChain::clear()
{
	m_writeBuffer = "";
	m_seekBuffer.clear();
	m_states.clear();

	m_visualiser.clearPoints();
}


void markovChain::constructVisualisation()
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
	m_visualiser.sound( g_RESOURCE_LOC + "songs/" + g_SOURCE );
}



void markovChain::update(const float _dt)
{
	m_visualiser.update(_dt);
}


void markovChain::visualise()
{
	m_visualiser.drawSpheres();
	m_visualiser.finalise();
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
	{
		g_SOURCE = source;
		loadSource(source);
	}
	pr.message( "Chain size : " + std::to_string(m_states.size()) + '\n' );
}


void markovChain::addContext(const notes &_state)
{
	fifoQueue(
				&m_seekBuffer,
				_state,
				m_order
				);
}


void markovChain::diagnoseNode(const std::string &_str)
{
	return;
}


float markovChain::getAverageNumConnections()
{
	size_t cons = 0;
	for(auto &i : m_states)
		cons += i.second.getNumConnections();
	cons = (float)cons / (float)m_states.size();
	return cons;
}


std::vector<notes> markovChain::getKeyFromContext()
{
	std::cout << "A\n";
	std::vector<notes> ret;

	std::cout << "B " << m_seekBuffer.size() << '\n';
	for(auto &state : m_seekBuffer)
	{
		std::cout << "		loop\n";
		ret.push_back(state);
		std::cout << "		post push\n";
	}
	return ret;
}


std::vector<notes> markovChain::getRandomContext()
{
	//Grab a random key from m_states
	auto it = m_states.begin();
	int advance = randInt(0, m_states.size());
	std::advance( it, advance );

	std::vector<notes> key = it->first;
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

void markovChain::loadSource(const std::string _path)
{
	resetBuffers();
	std::cout << "Reading from " << _path << '\n';

	sampler::initialiseAudio( 44100, 2 );
	sampler smpl( g_RESOURCE_LOC + "songs/" + _path );

	float time = 0.0f;

	bool done = false;
	std::vector<notes> states;
	while(!done)
	{
		//std::cout << "TIME : " << time << '\n';
		//Get raw fft
		std::vector<float> data = smpl.sampleAudio( time, gint("sample_width_bytes") );/*g_PARAM_SAMPLE_WIDTH*/

		done = gint("sample_width_bytes")/*g_PARAM_SAMPLE_WIDTH*/ + smpl.secsToBytes(time) >= smpl.get()->alen;
		time += 1.0f / gflt("sound_sample_frequency")/*g_PARAM_SAMPLE_TIMESTEP*/;

		std::vector<float> ni = getNoteVals( data );

		for(size_t i = 0; i < ni.size(); ++i)
			fifoQueue( &g_noteIntensity[i], ni[i], g_noteIntensityOrder );
		for(size_t i = 0; i < g_noteIntensity.size(); ++i)
		{
			//std::cout << "a " << a << '\n';
			g_averageNoteIntensity[i] = std::accumulate(g_noteIntensity[i].begin(), g_noteIntensity[i].end(), 0.0f) / g_noteIntensity[i].size();
		}

		std::vector<note> activeNotes = getActiveNotes(ni);
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
				m_states[m_seekBuffer].addConnection( states[i + 1] );
			}
		}
	}
	std::cout << "m_states size is " << m_states.size() << '\n';
}


void markovChain::write(size_t _wordCount)
{
	printer pr;
	resetBuffers();
	pr.message("Writing...", RED);

	//Word counting variables
	size_t curWord = 0;

	std::vector<notes> curKey = getRandomContext();

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
