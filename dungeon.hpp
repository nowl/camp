#ifndef __DUNGEON_HPP__
#define __DUNGEON_HPP__

#include <memory>
#include <vector>
#include <ostream>

#include "astar.hpp"

struct Room
{
    int dungeonPosX, dungeonPosY, posX, posY, width, height;
};

class Dungeon
{
public:
    Dungeon(unsigned int numRoomsX, unsigned int numRoomsY,
            unsigned int roomWidth, unsigned int roomHeight,
            unsigned int maxCellsX, unsigned int maxCellsY,
            unsigned int minCellsX, unsigned int minCellsY,
            unsigned int minDoors, unsigned int maxDoors,
            unsigned int minHallLen, unsigned int maxHallLen,
            float roomProb);

    inline void setCell(unsigned int x, unsigned int y, int val)
    {
        _dungeon[y*getWidth() + x] = val;
    }

    inline int getCell(unsigned int x, unsigned int y) const
    {
        return _dungeon[y*getWidth() + x];
    }

    int getWidth() const { return _numRoomsX * _roomWidth; }
    int getHeight() const { return _numRoomsY * _roomHeight; }

private:
    unsigned int _numRoomsX, _numRoomsY,
        _roomWidth, _roomHeight,
        _maxCellsX, _maxCellsY,
        _minCellsX, _minCellsY,
        _minDoors, _maxDoors,
        _minHallLen, _maxHallLen;    
    float _roomProb;

    std::unique_ptr<int[]> _dungeon;
    std::vector<Room> _rooms;
    friend std::ostream& operator<<(std::ostream& out, Dungeon &dng);

    void makeRoom(unsigned int x, unsigned int y);
    void makeHallInit(int sx, int sy);
    void makeHall(int sx, int sy, bool out_of_room, int incoming_dir);
    void makeHalls();
};

#endif  // __DUNGEON_HPP__
