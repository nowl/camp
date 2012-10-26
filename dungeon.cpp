#include <iostream>

#include "random.hpp"

#include "dungeon.hpp"

using namespace std;

std::ostream& operator<<(std::ostream& out, Dungeon &dng)
{
    for(int y=0; y<dng._dungeonHeight; y++)
    {
        for(int x=0; x<dng._dungeonWidth; x++)
        {
            out << dng.getCell(x, y) << "";
        }
        out << std::endl;
    }
    return out;
}

Dungeon::Dungeon(unsigned int numRoomsX, unsigned int numRoomsY,
                 unsigned int roomWidth, unsigned int roomHeight,
                 unsigned int maxCellsX, unsigned int maxCellsY,
                 unsigned int minCellsX, unsigned int minCellsY,
                 float roomProb)
  : _numRoomsX(numRoomsX), _numRoomsY(numRoomsY), _roomWidth(roomWidth), _roomHeight(roomHeight),
    _maxCellsX(maxCellsX), _maxCellsY(maxCellsY), _minCellsX(minCellsX), _minCellsY(minCellsY),
    _roomProb(roomProb), _dungeon(nullptr)
{
    _dungeonWidth = _numRoomsX * _roomWidth;
    _dungeonHeight = _numRoomsY * _roomHeight;
    _dungeon = std::unique_ptr<int[]>(new int[_dungeonWidth * _dungeonHeight]);

    for(int y=0; y<_dungeonHeight; y++)
        for(int x=0; x<_dungeonWidth; x++)
            setCell(x, y, 0);

    // make rooms
    for(unsigned int y=0; y<_numRoomsY; y++)
        for(unsigned int x=0; x<_numRoomsX; x++)
        {
            if( Random::f() < _roomProb )
            {
                Room room;
                room.dungeonPosX = x;
                room.dungeonPosY = y;
                room.width = Random::intMinMax(_minCellsX, _maxCellsX);
                room.height = Random::intMinMax(_minCellsY, _maxCellsY);
                room.posX = Random::intMinMax(0, _roomWidth - room.width);
                room.posY = Random::intMinMax(0, _roomHeight - room.height);

                _rooms.push_back(room);

                // draw room in dungeon
                for(unsigned int dx=room.dungeonPosX * _roomWidth + room.posX;
                    dx < room.dungeonPosX * _roomWidth + room.posX + room.width;
                    dx++)
                    for(unsigned int dy=room.dungeonPosY * _roomHeight + room.posY;
                        dy < room.dungeonPosY * _roomHeight + room.posY + room.height;
                        dy++)
                        setCell(dx, dy, 1);	
                /*
                cout << room.dungeonPosX << ", " << room.dungeonPosY << ", "
                     << room.width << ", " << room.height << ", "
                     << room.posX << ", " << room.posY << endl;
                */
            }
        }
}
