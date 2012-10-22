#include "game_components.hpp"
#include "sdl_driver.hpp"

#include "engine.hpp"
#include "player.hpp"
#include "game.hpp"
#include "message.hpp"
#include "colors.hpp"
#include "text.hpp"

GlobalRenderComponent::GlobalRenderComponent(Game *game)
    : game(game)
{
    addResponderType(Engine::RENDER_MESSAGE);
}

bool GlobalRenderComponent::respond(Message *message)
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

RenderComponent::RenderComponent(Renderable *renderable, Game *game)
    : _renderable(renderable), game(game)
{
    addResponderType("render");
}

bool RenderComponent::respond(Message *message)
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

TextRenderComponent::TextRenderComponent(Text *text, Game *game)
    : _text(text), game(game)
{
    addResponderType("render");
}

bool TextRenderComponent::respond(Message *message)
{
    if(message->type == Hash::hashString("render"))
    {
        auto payload = std::static_pointer_cast<RenderPayload>(message->payload);
        if(payload->level == _text->renderLevel)
        {            
            auto sdlDriver = game->engine.getSDLDriver();
            Colors c = Colors::Parse(_text->color);

            char imageName[2];
            for(int row=0; row<_text->numRows(); row++)
            {
                auto text = _text->getText(row);
                for(int i=0; i<text.length(); i++)
                {
                    sprintf(imageName, "%2x", text[i]);
                    GLuint texture = game->engine.getImageLoader()->get(imageName);
                    sdlDriver->drawImage(texture,
                                         _text->worldX + i * game->cellWidth,
                                         _text->worldY + row * game->cellHeight,
                                         game->cellWidth, game->cellHeight, c.r, c.g, c.b);
                    
                }
            }
        }
    }
    
    return false;
}

BoxOutlineRenderComponent::BoxOutlineRenderComponent(BoxRenderable *renderable, Game *game, bool flash)
    : _renderable(renderable), game(game), _flash(flash), _timer(0), _useFlashColor(false)
{
    addResponderType("render");
}

bool BoxOutlineRenderComponent::respond(Message *message)
{
    if(message->type == Hash::hashString("render"))
    {
        auto payload = std::static_pointer_cast<RenderPayload>(message->payload);
        if(payload->level == _renderable->renderLevel)
        {
            if(game->engine.getTick() > _timer)
            {
                _timer += _renderable->flashDuration;
                _useFlashColor = !_useFlashColor;
            }
            
            auto sdlDriver = game->engine.getSDLDriver();
            
            Colors c;
            if(_useFlashColor)
                c = Colors::Parse(_renderable->flashColor);
            else
                c = Colors::Parse(_renderable->color);

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

SelectorRenderComponent::SelectorRenderComponent(BoxRenderable *renderable, float selectorMargin, Game *game)
    :_renderable(renderable), selectorMargin(selectorMargin), game(game)
{
    addResponderType("render");
}

bool SelectorRenderComponent::respond(Message *message)
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
    
PlayerUIEventsComponent::PlayerUIEventsComponent(Player *player, Game *game) 
    : _player(player), game(game)
{
    addResponderType(Engine::UIEVENT_MESSAGE);
    addResponderType("collision");
}

bool PlayerUIEventsComponent::respond(Message *message)
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
        else if(p->event.type == SDL_MOUSEBUTTONDOWN)
        {
            // determine where mouse is
            auto mouseX = p->event.button.x;
            auto mouseY = p->event.button.y;
            _player->selectedCellRenderable.worldX = mouseX/game->cellWidth*game->cellWidth;
            _player->selectedCellRenderable.worldY = mouseY/game->cellHeight*game->cellHeight;
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
