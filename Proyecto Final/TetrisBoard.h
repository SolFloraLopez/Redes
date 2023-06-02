#include <SDL2/SDL.h>
#include <vector>
#include "Tetraminos.h"

enum TileType
{
    EMPTY = 0,
    RED = 1,
    ORANGE = 2,
    YELLOW = 3,
    GREEN = 4,
    BLUE = 5,
    PURPLE = 6,
    PINK = 7
};

class TetrisBoard 
{
public:
    static const uint8_t LENGTH = 10;
    static const uint8_t HEIGHT = 40;
    static const uint8_t TILE_SIZE = 20;

    TetrisBoard()
    {
        positions[0] = (HEIGHT / 2) * LENGTH + LENGTH / 2;

        GeneratePieces();
        ChangePiece();

        lastPivotPosition = positions[0];
        lastRotation = 0;
    };

    void DrawBoard(int offsetX, int offsetY, bool renderNext, SDL_Renderer* renderer);
    void DrawGrid(int offsetX, int offsetY, bool renderNext, SDL_Renderer* renderer);
    void DrawTiles(int offsetX, int offsetY, SDL_Renderer* renderer);
    void DrawNextPiece(int offsetX, int offsetY, SDL_Renderer* renderer);
    void SetToTileColor(uint8_t id, SDL_Renderer* renderer);

    bool CheckBoundsRight();
    bool CheckBoundsLeft();
    bool CheckBoundsDown();
    bool CheckColissions();
    int  CheckDistance(int piece);

    bool Rotate(int dir);
    bool CorrectPosition();

    bool CheckPieceSets();
    bool SetPiece();
    void ChangePiece();
    void GeneratePieces();

    bool CheckRows();
    void UpdateRows(int rows);

    void ClearTetramino();

    void Update();

    uint8_t tiles[400] = { 0 };
    int nextTetraminoID = 0;
    int positions[4];

    int lastPivotPosition;
    int lastRotation;

    int rowsDeleted = 0;
    int firstRow = 0;

    bool set = false;

    Tetramino* currentTetramino;

    std::vector<uint8_t> tetraminoPool;
};