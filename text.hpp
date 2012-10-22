#ifndef __TEXT_HPP__
#define __TEXT_HPP__

#include <string>
#include <vector>

#include "game.hpp"

class Text
{
public:
    Text(std::string text, unsigned int fitWidth);

    int numRows() const;
    std::string getText(int row) const;

    int renderLevel;
    float worldX, worldY;
    std::string color;

private:
    std::vector< std::string > _text;

    void splitText(std::string text, unsigned int fitWidth);
};

#endif  // __TEXT_HPP__
