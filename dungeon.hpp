#ifndef __DUNGEON_HPP__
#define __DUNGEON_HPP__

#include <memory>
#include <vector>
#include <ostream>

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
            float roomProb);

    inline void setCell(unsigned int x, unsigned int y, int val)
    {
        _dungeon[y*_dungeonWidth + x] = val;
    }

    inline int getCell(unsigned int x, unsigned int y) const
    {
        return _dungeon[y*_dungeonWidth + x];
    }

private:
    unsigned int _numRoomsX, _numRoomsY,
        _roomWidth, _roomHeight,
        _maxCellsX, _maxCellsY,
        _minCellsX, _minCellsY;
    float _roomProb;

    int _dungeonWidth, _dungeonHeight;
    std::unique_ptr<int[]> _dungeon;
    std::vector<Room> _rooms;
    friend std::ostream& operator<<(std::ostream& out, Dungeon &dng);
};

#endif  // __DUNGEON_HPP__
