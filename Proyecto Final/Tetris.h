#include <SDL2/SDL.h>
#include <sys/time.h>
#include <memory>

#include "Serializable.h"

#include "TetrisBoard.cc"
#include "Socket.cc"

class Message : public Serializable
{
public:
    static const size_t MESSAGE_SIZE = sizeof(uint8_t) + sizeof(int) + sizeof(char) * 8 + sizeof(char);

    enum MessageType
    {
        CONNECT     = 0,
        START       = 1,
        INPUT       = 2,
        END         = 3,
        DISCONNECT  = 4
    };

    Message(){};
    void to_bin();
    int from_bin(char * bobj);

    uint8_t type;
    int nextTetraminoID = 0;
    int pivotPos = 0;
    int rotation = 0;
    std::string playerName;
    char input = ' ';
};

class TetrisClient 
{
public:
    enum State
    {
        CONNECTING      = 0,
        WAITING         = 1,
        PLAYING         = 2,
        RESULT          = 3,
        DISCONNECT      = 4
    };

    TetrisClient(const char * s, const char * p, const char * name) : socket(s, p)    {
        socket.bind();
        playerName = name;
    };
    
    ~TetrisClient()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    };
    
    void Init();
    void Run();

    void Render();

    void ProcessInput(SDL_Event);
    void ProcessInputMsg();

    void Connect();
    void Disconnect();

    void SendInput(char i);

    void net_thread();

    int gameState = CONNECTING;

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;

    std::string playerName;

    TetrisBoard* board1;
    TetrisBoard* board2;

    bool set = false;
    bool input = false;
    bool run = false;

    char inputMsg = ' ';

    struct timeval initialTime, elapsedTime;

    int framesPerTick = 60;
    int minFramesPerTick = 5;
    int remainingFrames = 60;

    int ticksToSpeedUp = 10;
    int remainingTicks = 10;

private:

    Socket socket;
};

class TetrisServer
{
public:
    TetrisServer(const char * s, const char * p) : socket(s, p)
    {
        socket.bind();
    }

    void do_messages();

private:
    std::vector<std::unique_ptr<Socket>> clients;

    Socket socket;
};
