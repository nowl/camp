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
#include "text.hpp"

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
    player.renderable.color = Colors::Parse("white");
    player.boxRenderable.worldX = 0;
    player.boxRenderable.worldY = 0;
    player.boxRenderable.w = game->cellWidth;
    player.boxRenderable.h = game->cellHeight;
    player.boxRenderable.renderLevel = 2;
    player.boxRenderable.lineWidth = 1;
    player.boxRenderable.color = "yellow";
    player.selectedCellRenderable.worldX = 0;
    player.selectedCellRenderable.worldY = 0;
    player.selectedCellRenderable.w = game->cellWidth;
    player.selectedCellRenderable.h = game->cellHeight;
    player.selectedCellRenderable.renderLevel = 2;
    player.selectedCellRenderable.lineWidth = 1;
    player.selectedCellRenderable.color = "white";
    player.selectedCellRenderable.flashColor = "blue";
    player.selectedCellRenderable.flashDuration = 400;

    auto maxCellsX = game->engine.getSDLDriver()->getScreenWidth() / game->cellWidth - game->hudCellWidth;
    auto maxCellsY = game->engine.getSDLDriver()->getScreenHeight() / game->cellHeight;

    // text test
    Text text("this is a test of using a given fitWidth to break up text... testing, testing testing!");
    text.fitWidth = 20;
    text.renderLevel = 2;
    text.color = Colors(1, 1, 0);
    text.worldX = 10 * game->cellWidth;
    text.worldY = 10 * game->cellHeight;
    TextRenderComponent textRender(&text, game);

    RenderComponent playerRenderComponent(&player.renderable, game);
    SelectorRenderComponent playerSelectorOutlineComp(&player.boxRenderable, 6, game);

    PlayerUIEventsComponent playerUIEventsComponent(&player, game);
    BoxOutlineRenderComponent selectedCellComponent(&player.selectedCellRenderable, game, true);

    Walls walls;
    
    //for(int i=0; i<Random::intMinMax(1000, 2000); i++)
    for(int i=0; i<6000; i++)
    {
        std::string wallType;
        Colors color;
        if(Random::intMinMax(0,10) == 0)
        {
            wallType = "solid";
            color.r = Random::f();
            color.g = Random::f();
            color.b = Random::f();            
        }
        else
        {
            wallType = "water";
            color = Colors::Parse("blue");
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
