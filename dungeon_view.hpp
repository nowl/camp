#ifndef __DUNGEON_VIEW_HPP__
#define __DUNGEON_VIEW_HPP__

#include "dungeon.hpp"

class DungeonView
{
public:
    DungeonView(const Dungeon& dungeon)
        : _dungeon(dungeon)
    {}
   
    struct Rect
    {
        Rect()
            : ulx(0), uly(0), w(24), h(24)
        {}

        Rect(int ulx, int uly, int w, int h)
            : ulx(ulx), uly(uly), w(w), h(h)
        {}

        int ulx, uly, w, h;
    };

    inline void setViewingRect(const Rect& rect)
    {
        _viewing_rect = rect;
    }

    inline int getCell(unsigned int x, unsigned int y) const
    {
        // TODO: bounds checking?
        return _dungeon.getCell(x + _viewing_rect.ulx, y + _viewing_rect.uly);
    }

private:
    const Dungeon& _dungeon;
    Rect _viewing_rect;
    
};

#endif  // __DUNGEON_VIEW_HPP__
