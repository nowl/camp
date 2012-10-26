#include "colors.hpp"

Colors
Colors::Parse(const std::string& color)
{
    Colors c;
    
    if(color[0] == '#')
    {
        int r, g, b;
        sscanf(color.c_str(), "#%2x%2x%2x", &r, &g, &b);
        c.r = (float)r/255;
        c.g = (float)g/255;
        c.b = (float)b/255;
    } else {
        if(color == "white")
        { c.r=1; c.g=1; c.b=1; }
        else if(color == "blue")
        { c.r=0; c.g=0; c.b=1; }
        else if(color == "red")
        { c.r=1; c.g=0; c.b=0; }
        else if(color == "yellow")
        { c.r=1; c.g=1; c.b=0; }
    }
    
    return c;
}
