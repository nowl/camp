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
                if(_renderable->color == "white")
                    sdlDriver->drawImage(texture, _renderable->worldX, _renderable->worldY,
                                         9, 16, 1, 1, 1);
                else if(_renderable->color == "blue")
                    sdlDriver->drawImage(texture, _renderable->worldX, _renderable->worldY,
                                         9, 16, 0, 0, 1);
                else if(_renderable->color == "red")
                    sdlDriver->drawImage(texture, _renderable->worldX, _renderable->worldY,
                                         9, 16, 1, 0, 0);
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
                
                float r,g,b;
                if(_renderable->color == "white")
                { r=1; g=1; b=1; }
                
                auto x = _renderable->worldX;
                auto y = _renderable->worldY;
                auto w = _renderable->w;
                auto h = _renderable->h;
                auto lw = _renderable->lineWidth;
                sdlDriver->drawLine(x-1, y-1, x+w+1, y-1, lw, r, g, b);
                sdlDriver->drawLine(x-1, y-1, x-1, y+h+1, lw, r, g, b);
                sdlDriver->drawLine(x-1, y+h+1, x+w+1, y+h+1, lw, r, g, b);
                sdlDriver->drawLine(x+w+1, y-1, x+w+1, y+h+1, lw, r, g, b);
            }
        }
        return false;
    }
    
private:
    BoxRenderable *_renderable;
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
                                            _player->renderable.worldX + 9,
                                            _player->renderable.worldY);
                    Message::send("move", payload, Message::ASYNC);
                    if(_player->canMove)
                        _player->renderable.worldX += 9;
                    break;
                }
                case SDLK_KP4:
                {
                    _player->canMove = true;
                    MovementPayload payload("player",
                                            _player->renderable.worldX - 9,
                                            _player->renderable.worldY);
                    Message::send("move", payload, Message::ASYNC);
                    if(_player->canMove)
                        _player->renderable.worldX -= 9;
                    break;
                }
                case SDLK_KP8:
                {
                    _player->canMove = true;
                    MovementPayload payload("player",
                                            _player->renderable.worldX,
                                            _player->renderable.worldY - 16);
                    Message::send("move", payload, Message::ASYNC);
                    if(_player->canMove)
                        _player->renderable.worldY -= 16;
                    break;
                }
                case SDLK_KP2:
                {
                    _player->canMove = true;
                    MovementPayload payload("player",
                                            _player->renderable.worldX,
                                            _player->renderable.worldY + 16);
                    Message::send("move", payload, Message::ASYNC);
                    if(_player->canMove)
                        _player->renderable.worldY += 16;
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
                _player->boxRenderable.worldX = mouseX/9*9;
                _player->boxRenderable.worldY = mouseY/16*16;
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
    player.renderable.worldX = 9*5;
    player.renderable.worldY = 16*5;
    player.renderable.imageName = "player";
    player.renderable.renderLevel = 1;
    player.renderable.color = "white";
    player.boxRenderable.worldX = 0;
    player.boxRenderable.worldY = 0;
    player.boxRenderable.w = 9;
    player.boxRenderable.h = 16;
    player.boxRenderable.renderLevel = 2;
    player.boxRenderable.lineWidth = 1;
    player.boxRenderable.color = "white";

    RenderComponent playerRenderComponent(&player.renderable);
    BoxOutlineRenderComponent playerBoxOutlineComp(&player.boxRenderable);

    PlayerUIEventsComponent playerUIEventsComponent(&player);


    Walls walls;
    
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
        walls.renderEntities.push_back(Renderable(9*Random::intMinMax(0,70),
                                                  16*Random::intMinMax(0,40),
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
