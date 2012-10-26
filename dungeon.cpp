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
                 unsigned int minDoors, unsigned int maxDoors,
                 unsigned int minHallLen, unsigned int maxHallLen,
                 float roomProb)
  : _numRoomsX(numRoomsX), _numRoomsY(numRoomsY), _roomWidth(roomWidth), _roomHeight(roomHeight),
    _maxCellsX(maxCellsX), _maxCellsY(maxCellsY), _minCellsX(minCellsX), _minCellsY(minCellsY),
    _minDoors(minDoors), _maxDoors(maxDoors), _minHallLen(minHallLen), _maxHallLen(maxHallLen),
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
            if( Random::f() < _roomProb )
                makeRoom(x, y);

    // make random tunnels
    makeHalls();
}

void Dungeon::makeRoom(unsigned int x, unsigned int y)
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

void Dungeon::makeHall(int sx, int sy, bool out_of_room, int incoming_dir)
{
    // length
    int hallLen = Random::intMinMax(_minHallLen, _maxHallLen);

    // pick a direction
    enum Direction {
        NORTH = 0, EAST, SOUTH, WEST
    };

    int dir = Random::intMinMax(0, 3);

    // can't return in the incoming direction
    int opposite_dir = (dir + 2) % 4;
    if(incoming_dir == opposite_dir)
        dir = (dir + 1) % 4;

    int newY = 0, newX = 0;
    for(int i=1; i<=hallLen; i++)
    {
        switch(dir)
        {
        case NORTH:
            newX = sx;
            newY = sy - i;
            if(newY < 0)
                return;
            break;
        case EAST:
            newX = sx + i;
            newY = sy;
            if(newX >= _dungeonWidth)
                return;
            break;
        case SOUTH:
            newX = sx;
            newY = sy + i;
            if(newY >= _dungeonHeight)
                return;
            break;
        case WEST:
            newX = sx - i;
            newY = sy;
            if(newX < 0)
                return;
            break;
        }

        // check if we are out of the room yet
        if(!out_of_room && getCell(newX, newY) == 0)
            out_of_room = true;
        // check if we hit another room or hallway
        if(out_of_room && getCell(newX, newY) != 0)
            return;
        setCell(newX, newY, 2);
    }
    
    makeHall(newX, newY, out_of_room, dir);
}

void Dungeon::makeHalls()
{
    for(auto iter = _rooms.begin(); iter != _rooms.end(); iter++)
    {
        Room& room = *iter;
        int numDoors = Random::intMinMax(_minDoors, _maxDoors);
        for(int door_num = 0; door_num < numDoors; door_num++)
        {
            int tmp = room.dungeonPosX * _roomWidth + room.posX;
            int startX = Random::intMinMax(tmp, tmp + room.width - 1);
            tmp = room.dungeonPosY * _roomHeight + room.posY;
            int startY = Random::intMinMax(tmp, tmp + room.height - 1);
            
            makeHall(startX, startY, false, -1);
        }
    }
}
