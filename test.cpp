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

class FloatPayload : public Message::IPayload
{
public:
    float a;
};

void callFunction(std::function< void(std::string) > f)
{
    f("haha");
}



/*
void processMessage(Message *message)
{
    printf("processing message of type %d\n", message->type);
    auto p = std::static_pointer_cast<FloatPayload>(message->payload);
    //printf("float payload of %f\n", p->a);

    auto func = [&] (std::string str) {printf("float %s payload of %f\n", str.c_str(), p->a); };
    callFunction(func);
}
*/

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
            Message::send(NULL, "render", *message->payload.get(), Message::ASYNC);
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
                    _player->renderable.worldX += 9;
                    break;
                case SDLK_KP4:
                    _player->renderable.worldX -= 9;
                    break;
                case SDLK_KP8:
                    _player->renderable.worldY -= 16;
                    break;
                case SDLK_KP2:
                    _player->renderable.worldY += 16;
                    break;
                default:
                    break;
                }
        }

        return true;
    }

private:
    Player *_player;
};

int main(int argc, char *argv[])
{
    game->engine.getSDLDriver()->setVideoMode(1280, 1024);
    //float a = testing(14);
    //printf("%f\n", a);
    //const std::unique_ptr<SDLDriver>& sdl = e.getSDLDriver();
       
    //FloatPayload p;
    //p.a = 24;

    //Message::send<FloatPayload>(NULL, 4, p, Message::ASYNC);

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
    
    for(int i=0; i<Random::intMinMax(100, 1000); i++)
        walls.renderEntities.push_back(Renderable(9*Random::intMinMax(0,70),
                                                  16*Random::intMinMax(0,40),
                                                  "solid"));

    auto iter = walls.renderEntities.begin();
    for(; iter!=walls.renderEntities.end(); ++iter)
    {
        walls.renderComponents.push_back(std::unique_ptr<RenderComponent>(new RenderComponent(&*iter)));
    }

    game->engine.run();

    return 0;
}
