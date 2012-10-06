#include <SDL.h>

#include <cstdio>

#include "message.hpp"
#include "sdl_driver.hpp"
#include "component.hpp"
#include "log.hpp"
#include "hash.hpp"

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

static bool global_responder(Message *message, Entity *entity)
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

static bool render_responder(Message *message, Entity *entity)
{
    RenderEntity *re = static_cast<RenderEntity*>(entity);

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
                re->worldX += 9*2;
                break;
            case SDLK_KP4:
                re->worldX -= 9*2;
                break;
            case SDLK_KP8:
                re->worldY -= 16*2;
                break;
            case SDLK_KP2:
                re->worldY += 16*2;
                break;
            default:
                break;
            }
    }
    else if(message->type == Hash::hashString("render"))
    {
        auto sdlDriver = game->engine.getSDLDriver();
        GLuint texture = game->engine.getImageLoader()->get(game->imageNameCache.get(re->imageName));
        sdlDriver->drawImage(texture, re->worldX, re->worldY, 9, 16, 0, 0, 1);
    }

    return false;
}

static bool player_uievents(Message *message, Entity *entity)
{
    Player *player = static_cast<Player*>(entity);

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
                player->renderEntity.worldX += 9;
                break;
            case SDLK_KP4:
                player->renderEntity.worldX -= 9;
                break;
            case SDLK_KP8:
                player->renderEntity.worldY -= 16;
                break;
            case SDLK_KP2:
                player->renderEntity.worldY += 16;
                break;
            default:
                break;
            }
    }

    return true;
}

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
    
    Component systemRenderComp(global_responder);
    systemRenderComp.addResponderType(Engine::RENDER_MESSAGE);

    Entity systemEntity;
    systemRenderComp.addEntity(&systemEntity);

    Component renderComp(render_responder);
    //renderComp.addResponderType(Engine::UIEVENT_MESSAGE);
    //comp1.addResponderType(Engine::UPDATE_MESSAGE);
    //renderComp.addResponderType(Engine::RENDER_MESSAGE);
    renderComp.addResponderType("render");

    Component playerUpdateComp(player_uievents);
    playerUpdateComp.addResponderType(Engine::UIEVENT_MESSAGE);

    Player player;
    player.renderEntity.worldX = 9*5;
    player.renderEntity.worldY = 16*5;
    player.renderEntity.imageName = "player";
    renderComp.addEntity(&player.renderEntity);
    playerUpdateComp.addEntity(&player);

    Walls walls;
    walls.renderEntities.push_back(RenderEntity(9, 0, "solid"));
    walls.renderEntities.push_back(RenderEntity(9*2, 0, "solid"));
    walls.renderEntities.push_back(RenderEntity(9*4, 0, "solid"));
    walls.renderEntities.push_back(RenderEntity(9*7, 16*2, "water"));

    auto iter = walls.renderEntities.begin();
    for(; iter!=walls.renderEntities.end(); ++iter)
        renderComp.addEntity(&*iter);

    game->engine.run();

    return 0;
}
