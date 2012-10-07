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
            Message::send("render", *message->payload.get(), Message::ASYNC);
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
            auto sdlDriver = game->engine.getSDLDriver();
            GLuint texture = game->engine.getImageLoader()->get(game->imageNameCache.get(_renderable->imageName));
            sdlDriver->drawImage(texture, _renderable->worldX, _renderable->worldY, 9, 16, 0, 0, 1);
        }

        return false;
    }
    
private:
    Renderable *_renderable;
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
            if(p->event.type == SDL_KEYDOWN)
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

    RenderComponent playerRenderComponent(&player.renderable);

    PlayerUIEventsComponent playerUIEventsComponent(&player);


    Walls walls;
    
    for(int i=0; i<Random::intMinMax(1000, 2000); i++)
        walls.renderEntities.push_back(Renderable(9*Random::intMinMax(0,70),
                                                  16*Random::intMinMax(0,40),
                                                  "solid"));

    auto iter = walls.renderEntities.begin();
    for(; iter!=walls.renderEntities.end(); ++iter)
    {
        walls.renderComponents.push_back(std::unique_ptr<RenderComponent>(new RenderComponent(&*iter)));
        walls.updateComponents.push_back(std::unique_ptr<WallUpdateComponent>(new WallUpdateComponent(&*iter)));
    }

    game->engine.run();

    return 0;
}
