#ifndef __WALLS_HPP__
#define __WALLS_HPP__

#include <vector>
#include <memory>

#include "game_components.hpp"

class RenderComponent;

class WallUpdateComponent : public Component {
public:
    WallUpdateComponent(Renderable *renderable)
        : _renderable(renderable)
    {
        addResponderType("move");
    }

    virtual bool respond(Message *message)
    {
        if(message->type == Hash::hashString("move"))
        {
            auto payload = std::static_pointer_cast<MovementPayload>(message->payload);
            if(payload->worldX == _renderable->worldX &&
               payload->worldY == _renderable->worldY)
            {
                CollisionPayload pl("wall", payload->worldX, payload->worldY);
                Message::send("collision", pl, Message::ASYNC);
            }
        }
        
        return false;
    }

private:
    Renderable *_renderable;
};

class Walls {
public:
    std::vector<Renderable> renderEntities;
    std::vector< std::unique_ptr<RenderComponent> > renderComponents;
    std::vector< std::unique_ptr<WallUpdateComponent> > updateComponents;
};

#endif  // __WALLS_HPP__
