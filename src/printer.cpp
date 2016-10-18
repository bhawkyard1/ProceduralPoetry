#include <iostream>
#include "printer.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

void printer::message(const std::string &_msg, const textColour _col)
{
    setColour( _col );
    std::cout << _msg;
}

void printer::message(const std::string &_msg)
{
    std::cout << _msg;
}

void printer::setColour(const textColour _col)
{
#ifdef _WIN32
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), _col );
#endif
}
