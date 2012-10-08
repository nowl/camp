#ifndef __GAME_COMPONENTS_HPP__
#define __GAME_COMPONENTS_HPP__

#include <string>

#include "message.hpp"

class Renderable {
public:    
    float worldX, worldY;
    std::string imageName;
    std::string color;
    int renderLevel;

    Renderable() {}
    Renderable(float x, float y, std::string name, std::string color)
        : worldX(x), worldY(y), imageName(name), color(color),
          renderLevel(0)
    {}
};

class BoxRenderable {
public:    
    float worldX, worldY, w, h;
    float lineWidth;
    std::string color;
    int renderLevel;
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

struct RenderPayload : public Message::IPayload {
    int level;

    RenderPayload(int level)
        : level(level)
    {}
};

#endif  // __GAME_COMPONENTS_HPP__
