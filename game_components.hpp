#ifndef __GAME_COMPONENTS_HPP__
#define __GAME_COMPONENTS_HPP__

#include <string>

class Renderable {
public:    
    float worldX, worldY;
    std::string imageName;

    Renderable() {}
    Renderable(float x, float y, std::string name)
        : worldX(x), worldY(y), imageName(name)
    {}
    virtual ~Renderable() {};    
};

#endif  // __GAME_COMPONENTS_HPP__
