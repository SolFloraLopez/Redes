#include <stdlib.h>
#include <utility>
#include <array>
#include <iostream>

class Tetramino
{
public:

    const std::array<std::array<std::pair<int, int>, 3>, 7> positions =
    {
        { { { { 1, 0 }, { 0, 1 }, { 1, 1 } } },        // Square
        { { { -1, 0 }, { 1, 0 }, { 2, 0 } } },       // I
        { { { 0, -1 }, { 0, 1 }, { 1, 1 } } },       // L
        { { { 0, -1 }, { 0, 1 }, { -1, 1 } } },      // Reverse L
        { { { 1, -1 }, { 1, 0 }, { 0, 1 } } },       // N
        { { { 0, -1 }, { 1, 0 }, { 1, 1 } } },       // Reverse N
        { { { 0, -1 }, { 1, 0 }, { 0, 1 } } } }      // T
    };

    enum Directions
    {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3
    };

    Tetramino(uint8_t id) 
    {
        _id = id;
        initialPos = positions[_id];
        UpdatePieceRotation();
    };

    void Rotate(int deltaDir) 
    {
        rotation += deltaDir; 
        rotation = rotation % 4;
        if(rotation < 0) rotation = WEST;

        if(_id > 0) UpdatePieceRotation();
    };

    void UpdatePieceRotation()
    {
        for(int i = 0; i < 3; i++) 
        {
            switch (rotation)
            {
                case NORTH:
                    currentPos[i].first = initialPos[i].first; 
                    currentPos[i].second = initialPos[i].second;
                    break;
                case EAST:
                    currentPos[i].first = -initialPos[i].second; 
                    currentPos[i].second = initialPos[i].first;
                    break;
                case SOUTH:
                    currentPos[i].first = -initialPos[i].first; 
                    currentPos[i].second = -initialPos[i].second;
                    break;
                case WEST:
                    currentPos[i].first = initialPos[i].second; 
                    currentPos[i].second = -initialPos[i].first;
                    break;
                default:
                    break;
            }
        }
    };

    void GetPiecePositions(int boardLength, int (&positions)[4]) 
    {
        for(int i = 1; i < 4; i++) positions[i] = positions[0] + currentPos[i - 1].first + currentPos[i - 1].second * boardLength;
    }

    std::array<std::pair<int, int>, 3> initialPos;
    std::array<std::pair<int, int>, 3> currentPos;

    int rotation = 0;
    u_int8_t _id;
    
};