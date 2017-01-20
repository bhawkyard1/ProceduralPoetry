//----------------------------------------------------------------------------------------------------------------------
/// \file printer.hpp
/// \brief This file contains the 'printer' class, and related types.
/// \author Ben Hawkyard
/// \version 1.0
/// \date 19/01/17
/// Revision History :
/// This is an initial version used for the program.
/// \class printer
/// \brief A wrapper around windows stdout functions, I use it to colour terminal output. This currently has no effect
/// on linux.
//----------------------------------------------------------------------------------------------------------------------

#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <iostream>
#include <string>

enum textColour{
	BLACK, BLUE, GREEN, CYAN, RED, PURPLE, DARKYELLOW, WHITE, GREY, BRIGHTBLUE, BRIGHTGREEN, BRIGHTCYAN, BRIGHTRED, PINK, YELLOW, BRIGHTWHITE
};

class printer
{
public:
	~printer();
	void br() {std::cout << '\n';}
	void message(const std::string &_msg, const textColour _col);
	void message(const std::string &_msg);
private:
	void setColour(const textColour _col);
};

#endif
