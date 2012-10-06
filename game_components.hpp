#ifndef __GAME_COMPONENTS_HPP__
#define __GAME_COMPONENTS_HPP__

#include <string>

#include "entity.hpp"

class RenderEntity : public Entity {
public:    
    float worldX, worldY;
    std::string imageName;

    RenderEntity() {}
    RenderEntity(float x, float y, std::string name)
        : worldX(x), worldY(y), imageName(name)
    {}
    virtual ~RenderEntity() {};    
};

#endif  // __GAME_COMPONENTS_HPP__
