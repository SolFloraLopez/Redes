#include <SDL2/SDL.h>
#include <sys/time.h>
#include <memory>

#include "Serializable.h"

#include "TetrisBoard.cc"
#include "Socket.cc"

class Message : public Serializable
{
public:
    static const size_t MESSAGE_SIZE = sizeof(uint8_t) + sizeof(int) * 4 + sizeof(char) * 8 ;

    enum MessageType
    {
        CONNECT     = 0,
        START       = 1,
        DATA        = 2,
        END         = 3,
        DISCONNECT  = 4
    };

    Message(){};
    void to_bin();
    int from_bin(char * bobj);

    uint8_t type;
    int currentTetraminoID = 0;
    int pivotPos = 0;
    int rotation = 0;
    int set = 0;
    std::string playerName;
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
    void ProcessMsg();

    void EndGame();
    void Restart();
    void Connect();
    void Disconnect();

    void SendData(bool set);

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

    Message lastMsg;
    bool processed = false;

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
  enum State
  {
    CONNECTING = 0,
    PLAYING = 1,
    RESULT = 2,
    DISCONNECT = 3
  };

  TetrisServer(const char *s, const char *p) : socket(s, p)
  {
      socket.bind();
    }

    void do_messages();

private:
    std::vector<std::unique_ptr<Socket>> clients;
    std::vector<std::string> names;

    int state = CONNECTING;

    Socket socket;
};
