#include <SDL.h>

#include <cstdio>

#include "message.hpp"
#include "sdl_driver.hpp"
#include "component.hpp"
#include "log.hpp"
#include "hash.hpp"
#include "random.hpp"

#include "cp437.hpp"
#include "game.hpp"
#include "player.hpp"
#include "walls.hpp"
#include "game_components.hpp"
#include "colors.hpp"

static Game *game = Game::Instance();

static const int NumRenderLevels = 3;

class GlobalRenderComponent : public Component {
public:
    GlobalRenderComponent()
    {
        addResponderType(Engine::RENDER_MESSAGE);
    }

    virtual bool respond(Message *message)
    {
        if(message->type == Hash::hashString(Engine::RENDER_MESSAGE))
        {
            auto sdlDriver = game->engine.getSDLDriver();
            sdlDriver->preRender();
            for(int i=0; i<NumRenderLevels; i++)
                Message::send("render", RenderPayload(i), Message::ASYNC);
            sdlDriver->postRender();
        }
        
        return true;
    }
};

class RenderComponent : public Component {
public:
    RenderComponent(Renderable *renderable)
        :_renderable(renderable)
    {
        addResponderType("render");
    }

    virtual bool respond(Message *message)
    {
        if(message->type == Hash::hashString("render"))
        {
            auto payload = std::static_pointer_cast<RenderPayload>(message->payload);
            if(payload->level == _renderable->renderLevel)
            {
                auto sdlDriver = game->engine.getSDLDriver();
                GLuint texture = game->engine.getImageLoader()->get(game->imageNameCache.get(_renderable->imageName));

                Colors c = Colors::Parse(_renderable->color);                
                sdlDriver->drawImage(texture, _renderable->worldX, _renderable->worldY,
                                     game->cellWidth, game->cellHeight, c.r, c.g, c.b);
            }
        }

        return false;
    }
    
private:
    Renderable *_renderable;
};

class BoxOutlineRenderComponent : public Component {
public:
    BoxOutlineRenderComponent(BoxRenderable *renderable)
        :_renderable(renderable)
    {
        addResponderType("render");
    }

    virtual bool respond(Message *message)
    {
        if(message->type == Hash::hashString("render"))
        {
            auto payload = std::static_pointer_cast<RenderPayload>(message->payload);
            if(payload->level == _renderable->renderLevel)
            {
                auto sdlDriver = game->engine.getSDLDriver();
                
                auto c = Colors::Parse(_renderable->color);
                auto x = _renderable->worldX;
                auto y = _renderable->worldY;
                auto w = _renderable->w;
                auto h = _renderable->h;
                auto lw = _renderable->lineWidth;
                sdlDriver->drawLine(x-1, y-1, x+w+1, y-1, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x-1, y-1, x-1, y+h+1, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x-1, y+h+1, x+w+1, y+h+1, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x+w+1, y-1, x+w+1, y+h+1, c.r, c.g, c.b, lw);
            }
        }
        return false;
    }
    
private:
    BoxRenderable *_renderable;
};

class SelectorRenderComponent : public Component {
public:
    SelectorRenderComponent(BoxRenderable *renderable, float selectorMargin)
        :_renderable(renderable), selectorMargin(selectorMargin)
    {
        addResponderType("render");
    }

    virtual bool respond(Message *message)
    {
        if(message->type == Hash::hashString("render"))
        {
            auto payload = std::static_pointer_cast<RenderPayload>(message->payload);
            if(payload->level == _renderable->renderLevel)
            {
                auto sdlDriver = game->engine.getSDLDriver();
                
                auto c = Colors::Parse(_renderable->color);
                auto x = _renderable->worldX;
                auto y = _renderable->worldY;
                auto w = _renderable->w;
                auto h = _renderable->h;
                auto lw = _renderable->lineWidth;
                auto s = selectorMargin;
                sdlDriver->drawLine(x-1, y-1, x+w/s, y-1, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x+w-w/s, y-1, x+w+1, y-1, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x-1, y-1, x-1, y+h/s, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x-1, y+h-h/s, x-1, y+h+1, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x-1, y+h+1, x+w/s, y+h+1, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x+w-w/s, y+h+1, x+w+1, y+h+1, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x+w+1, y-1, x+w+1, y+h/s, c.r, c.g, c.b, lw);
                sdlDriver->drawLine(x+w+1, y+h-h/s, x+w+1, y+h+1, c.r, c.g, c.b, lw);
            }
        }
        return false;
    }
    
private:
    BoxRenderable *_renderable;
    float selectorMargin;
};

class PlayerUIEventsComponent : public Component {
public:
    PlayerUIEventsComponent(Player *player) 
        : _player(player)
    {
        addResponderType(Engine::UIEVENT_MESSAGE);
        addResponderType("collision");
    }

    virtual bool respond(Message *message)
    {
        if(message->type == Hash::hashString(Engine::UIEVENT_MESSAGE))
        {
            auto p = std::static_pointer_cast<SDLEventPayload>(message->payload);
            
            if(p->event.type == SDL_QUIT ||
               (p->event.type == SDL_KEYDOWN && p->event.key.keysym.sym == SDLK_ESCAPE))
                game->engine.quit();
            else if(p->event.type == SDL_KEYDOWN)
            {
                switch(p->event.key.keysym.sym)
                {
                case SDLK_KP6:
                {
                    _player->canMove = true;
                    MovementPayload payload("player",
                                            _player->renderable.worldX + game->cellWidth,
                                            _player->renderable.worldY);
                    Message::send("move", payload, Message::ASYNC);
                    if(_player->canMove)
                        _player->renderable.worldX += game->cellWidth;
                    break;
                }
                case SDLK_KP4:
                {
                    _player->canMove = true;
                    MovementPayload payload("player",
                                            _player->renderable.worldX - game->cellWidth,
                                            _player->renderable.worldY);
                    Message::send("move", payload, Message::ASYNC);
                    if(_player->canMove)
                        _player->renderable.worldX -= game->cellWidth;
                    break;
                }
                case SDLK_KP8:
                {
                    _player->canMove = true;
                    MovementPayload payload("player",
                                            _player->renderable.worldX,
                                            _player->renderable.worldY - game->cellHeight);
                    Message::send("move", payload, Message::ASYNC);
                    if(_player->canMove)
                        _player->renderable.worldY -= game->cellHeight;
                    break;
                }
                case SDLK_KP2:
                {
                    _player->canMove = true;
                    MovementPayload payload("player",
                                            _player->renderable.worldX,
                                            _player->renderable.worldY + game->cellHeight);
                    Message::send("move", payload, Message::ASYNC);
                    if(_player->canMove)
                        _player->renderable.worldY += game->cellHeight;
                    break;
                }
                default:
                    break;
                }
            }
            else if(p->event.type == SDL_MOUSEMOTION)
            {
                // determine where mouse is
                auto mouseX = p->event.motion.x;
                auto mouseY = p->event.motion.y;
                _player->boxRenderable.worldX = mouseX/game->cellWidth*game->cellWidth;
                _player->boxRenderable.worldY = mouseY/game->cellHeight*game->cellHeight;
            }
        }
        else if(message->type == Hash::hashString("collision"))
        {
            auto payload = std::static_pointer_cast<CollisionPayload>(message->payload);
            if(payload->source == "wall")
                _player->canMove = false;
        }

        return true;
    }

private:
    Player *_player;
};

int main(int argc, char *argv[])
{
    game->engine.getSDLDriver()->setVideoMode(1280, 1024);

    load_cp437();
    
    game->imageNameCache.cache("player", "40");
    game->imageNameCache.cache("water", "f7");
    game->imageNameCache.cache("solid", "db");
    
    GlobalRenderComponent systemRenderComp;

    Player player;
    player.renderable.worldX = game->cellWidth*5;
    player.renderable.worldY = game->cellHeight*5;
    player.renderable.imageName = "player";
    player.renderable.renderLevel = 1;
    player.renderable.color = "white";
    player.boxRenderable.worldX = 0;
    player.boxRenderable.worldY = 0;
    player.boxRenderable.w = game->cellWidth;
    player.boxRenderable.h = game->cellHeight;
    player.boxRenderable.renderLevel = 2;
    player.boxRenderable.lineWidth = 1;
    player.boxRenderable.color = "yellow";

    RenderComponent playerRenderComponent(&player.renderable);
    SelectorRenderComponent playerSelectorOutlineComp(&player.boxRenderable, 6);

    PlayerUIEventsComponent playerUIEventsComponent(&player);


    Walls walls;

    auto maxCellsX = game->engine.getSDLDriver()->getScreenWidth() / game->cellWidth;
    auto maxCellsY = game->engine.getSDLDriver()->getScreenHeight() / game->cellHeight;
    
    for(int i=0; i<Random::intMinMax(1000, 2000); i++)
    {
        std::string wallType, color;
        if(Random::intMinMax(0,1) == 0)
        {
            wallType = "solid";
            color = "red";
        }
        else
        {
            wallType = "water";
            color = "blue";
        }
        walls.renderEntities.push_back(Renderable(game->cellWidth*Random::intMinMax(0,maxCellsX-1),
                                                  game->cellHeight*Random::intMinMax(0,maxCellsY-1),
                                                  wallType,
                                                  color));
    }

    auto iter = walls.renderEntities.begin();
    for(; iter!=walls.renderEntities.end(); ++iter)
    {
        walls.renderComponents.push_back(std::unique_ptr<RenderComponent>(new RenderComponent(&*iter)));
        walls.updateComponents.push_back(std::unique_ptr<WallUpdateComponent>(new WallUpdateComponent(&*iter)));
    }

    game->engine.run();

    return 0;
}
