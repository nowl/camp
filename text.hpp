#ifndef __TEXT_HPP__
#define __TEXT_HPP__

#include <string>
#include <vector>

#include "game.hpp"
#include "colors.hpp"

class Text
{
public:
    Text(std::string text);

    void setText(std::string text);

    int numRows() const;
    std::string getText(int row) const;

    unsigned int fitWidth;
    int renderLevel;
    float worldX, worldY;
    Colors color;

private:
    std::vector< std::string > _text;

    void splitText(std::string text);
};

#endif  // __TEXT_HPP__
