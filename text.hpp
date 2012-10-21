#ifndef __TEXT_HPP__
#define __TEXT_HPP__

#include <string>

#include "game.hpp"

class Text
{
public:
    Text(std::string text, unsigned int fitWidth, int renderLevel);

    void draw(float x, float y, float r, float g, float b);

    int renderLevel;
};

#endif  // __TEXT_HPP__
