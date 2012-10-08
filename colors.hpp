#ifndef __COLORS_HPP__
#define __COLORS_HPP__

#include <string>

struct Colors
{
public:
    float r, g, b;

    static Colors Parse(std::string color);
};

#endif  // __COLORS_HPP__
