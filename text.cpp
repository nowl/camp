#include "sdl_driver.hpp"

#include "text.hpp"

Text::Text(std::string text)
    : fitWidth(20), renderLevel(0), worldX(0), worldY(0), color(Colors::Parse("white"))
{
    setText(text);
}

void Text::setText(std::string text)
{
    _text.clear();
    splitText(text);
}

int Text::numRows() const
{
    return _text.size();
}

std::string Text::getText(int row) const
{
    return _text[row];
}

void Text::splitText(std::string text)
{
    unsigned int lastSpace = 0;
    int llastSpace = -1;
    int row = 0;
    for(unsigned int i=0; i<text.length(); i++)
    {
        if(i-llastSpace > fitWidth+1)
        {
            _text.push_back(text.substr(llastSpace+1, lastSpace-llastSpace-1));
            printf("l last space = %d\n", llastSpace);
            printf("last space = %d\n", lastSpace);
            row++;
            llastSpace = lastSpace;
        }

        if(text[i] == ' ')
            lastSpace = i;
    }
    
    _text.push_back(text.substr(llastSpace+1));
}
