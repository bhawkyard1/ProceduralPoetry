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

class markovChain
{
public:
	markovChain(size_t _order);

	void addNode(const markovState &_node);

	void clear();

	void diagnoseNode( const std::string &_str );

	float getAverageNumConnections();

	visualiser * getVis() {return &m_visualiser;}

	std::vector<notes> getRandomContext();

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
	void addContext(const notes &_state);
	std::vector<notes> getKeyFromContext();
	void resetBuffers() {m_writeBuffer = ""; m_seekBuffer.clear();}
	void load();

	size_t m_order;

	//We write into this buffer, and can write to disk/display to screen.
	std::string m_writeBuffer;

	//Traversing m_states will only return one string per step. This is written into a buffer (FIFO queue) to give context for the next step.
	std::vector<notes> m_seekBuffer;

	//This is the guts of the chain, the vector of T (which will be order in length) is the key, and a markov state takes the value.
	std::map< std::vector<notes>, markovState, notesComp > m_states;

	visualiser m_visualiser;
};

#endif
