#include "colors.hpp"

Colors
Colors::Parse(std::string color)
{
    Colors c;
    if(color == "white")
    { c.r=1; c.g=1; c.b=1; }
    else if(color == "blue")
    { c.r=0; c.g=0; c.b=1; }
    else if(color == "red")
    { c.r=1; c.g=0; c.b=0; }
    else if(color == "yellow")
    { c.r=1; c.g=1; c.b=0; }
    
    return c;
}
