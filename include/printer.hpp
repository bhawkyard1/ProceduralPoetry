#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <string>

enum textColour{
    BLACK, BLUE, GREEN, CYAN, RED, PURPLE, DARKYELLOW, WHITE, GREY, BRIGHTBLUE, BRIGHTGREEN, BRIGHTCYAN, BRIGHTRED, PINK, YELLOW, BRIGHTWHITE
};

class printer
{
public:
    void br() {std::cout << '\n';}
    void message(const std::string &_msg, const textColour _col);
    void message(const std::string &_msg);
private:
    void setColour(const textColour _col);
};

#endif
