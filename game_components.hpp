#ifndef __GAME_COMPONENTS_HPP__
#define __GAME_COMPONENTS_HPP__

#include <string>

#include "message.hpp"

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

struct MovementPayload : public Message::IPayload {
    std::string source;
    float worldX, worldY;

    MovementPayload(std::string source, float worldX, float worldY)
        : source(source), worldX(worldX), worldY(worldY)
    {}
};

struct CollisionPayload : public Message::IPayload {
    std::string source;
    float worldX, worldY;

    CollisionPayload(std::string source, float worldX, float worldY)
        : source(source), worldX(worldX), worldY(worldY)
    {}
};

#endif  // __GAME_COMPONENTS_HPP__
