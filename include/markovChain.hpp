#ifndef MARKOVCHAIN_HPP
#define MARKOVCHAIN_HPP

#include <map>
#include <queue>
#include <string>

#include "markovState.hpp"
#include "visualiser.hpp"

using mKey = std::vector< std::string >;

class markovChain
{
public:
    markovChain(size_t _order);

    void addNode(const markovState &_node);

    void diagnoseNode( const std::string &_str );

    float getAverageNumConnections();

    mKey getRandomContext();

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
    void visualise(const float _dt);

    void resetPos() {m_visualiser.resetPos();}

private:
    void addContext(const std::string &_str);
    mKey getKeyFromContext();
    void resetBuffers() {m_writeBuffer = ""; m_seekBuffer.clear();}
    void load();

    size_t m_order;

    //We write into this buffer, and can write to disk/display to screen.
    std::string m_writeBuffer;

    //Traversing m_states will only return one string per step. This is written into a buffer (FIFO queue) to give context for the next step.
    mKey m_seekBuffer;

    //This is the guts of the chain, the vector of strings (which will be order in length) is the string associates
    std::map< mKey, markovState > m_states;

    visualiser m_visualiser;
};

#endif
