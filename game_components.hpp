#ifndef __GAME_COMPONENTS_HPP__
#define __GAME_COMPONENTS_HPP__

#include <string>

#include "component.hpp"
#include "message.hpp"
#include "colors.hpp"

class Game;
class Player;
class Text;

static const int NumRenderLevels = 3;

class Renderable {
public:    
    float worldX, worldY;
    std::string imageName;
    Colors color;
    int renderLevel;

    Renderable() {}
    Renderable(float x, float y, std::string name, Colors color)
        : worldX(x), worldY(y), imageName(name), color(color),
          renderLevel(0)
    {}
};

class BoxRenderable {
public:    
    float worldX, worldY, w, h;
    float lineWidth;
    std::string color, flashColor;
    unsigned long flashDuration;
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

// components

class GlobalRenderComponent : public Component {
public:
    GlobalRenderComponent(Game *game);
    virtual bool respond(Message *message);
private:
    Game *game;
};

class RenderComponent : public Component {
public:
    RenderComponent(Renderable *renderable, Game *game);
    virtual bool respond(Message *message);
private:
    Renderable *_renderable;
    Game *game;
};

class TextRenderComponent : public Component {
public:
    TextRenderComponent(Text *text, Game *game);
    virtual bool respond(Message *message);
private:
    Text *_text;
    Game *game;
};

class BoxOutlineRenderComponent : public Component {
public:
    BoxOutlineRenderComponent(BoxRenderable *renderable, Game *game, bool flash=false);
    virtual bool respond(Message *message);
private:
    BoxRenderable *_renderable;
    Game *game;
    bool _flash;
    int _timer;
    bool _useFlashColor;
};

class SelectorRenderComponent : public Component {
public:
    SelectorRenderComponent(BoxRenderable *renderable, float selectorMargin, Game *game);
    virtual bool respond(Message *message);
private:
    BoxRenderable *_renderable;
    float selectorMargin;
    Game *game;
};

class PlayerUIEventsComponent : public Component {
public:
    PlayerUIEventsComponent(Player *player, Game *game);
    virtual bool respond(Message *message);
private:
    Player *_player;
    Game *game;
};

#endif  // __GAME_COMPONENTS_HPP__
