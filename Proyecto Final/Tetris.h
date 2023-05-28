#include SDL2SDL.h
#include systime.h
#include Serializable.h
#include Tetraminos.h

class TetrisBoard  public Serializable
{
public
    static const uint8_t LENGTH = 10;
    static const uint8_t HEIGHT = 40;
    static const uint8_t TILE_SIZE = 20;

    TetrisBoard(){};

    void to_bin() {};
    int from_bin(char  data) {return 0;};

    enum TileType
    {
        EMPTY   = 0,
        RED     = 1,
        ORANGE  = 2,
        YELLOW  = 3,
        GREEN   = 4,
        BLUE    = 5,
        PURPLE  = 6,
        PINK    = 7
    };

    uint8_t tiles[400] = { 0 };
    char playerName;
};

class TetrisClient 
{
public
    TetrisClient(){};
    ~TetrisClient()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    };
    
    void Init();
    void Run();

    void Render();
    void DrawBoard(int offsetX, int offsetY, TetrisBoard b);
    void DrawGrid(int offsetX, int offsetY, TetrisBoard b);
    void DrawTiles(int offsetX, int offsetY, TetrisBoard b);

    void ProcessInput(SDL_Event);

    bool CheckBoundsRight();
    bool CheckBoundsLeft();
    bool CheckBoundsDown();
    bool CheckColissions();

    SDL_Window  window = nullptr;
    SDL_Renderer  renderer = nullptr;

    Tetramino currentTetramino;
    TetrisBoard board1;
    TetrisBoard board2;
    int positions[4];
    int lastPivotPosition;
    int lastRotation;

    bool run = false;
    struct timeval initialTime, elapsedTime;
};