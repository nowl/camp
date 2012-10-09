#include <SDL.h>

#include <cstdio>

#include "message.hpp"
#include "sdl_driver.hpp"
#include "log.hpp"
#include "hash.hpp"
#include "random.hpp"

#include "cp437.hpp"
#include "player.hpp"
#include "walls.hpp"
#include "game_components.hpp"
#include "game.hpp"

static Game *game = Game::Instance();

int main(int argc, char *argv[])
{
    game->engine.getSDLDriver()->setVideoMode(1280, 1024);

    load_cp437();
    
    game->imageNameCache.cache("player", "40");
    game->imageNameCache.cache("water", "f7");
    game->imageNameCache.cache("solid", "db");
    
    GlobalRenderComponent systemRenderComp(game);

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

    RenderComponent playerRenderComponent(&player.renderable, game);
    SelectorRenderComponent playerSelectorOutlineComp(&player.boxRenderable, 6, game);

    PlayerUIEventsComponent playerUIEventsComponent(&player, game);


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
        walls.renderComponents.push_back(std::unique_ptr<RenderComponent>(new RenderComponent(&*iter, game)));
        walls.updateComponents.push_back(std::unique_ptr<WallUpdateComponent>(new WallUpdateComponent(&*iter)));
    }

    game->engine.run();

    return 0;
}
