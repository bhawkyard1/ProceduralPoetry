#ifndef FONT_H
#define FONT_H

#include <string>

class font
{
public:
    font(const std::string &_path);
    void loadFontSpriteSheet(std::string _name, std::string _path, int _size);
};

#endif // FONT_H
