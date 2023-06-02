#include <thread>
#include "Tetris.cc"

int main(int argc, char **argv)
{    
    TetrisClient *tetrisClient = new TetrisClient(argv[1], argv[2], argv[3]);
    tetrisClient->Init();

    //std::cout << "Initialized" << std::endl;

    std::thread net_thread([&tetrisClient](){ tetrisClient->net_thread(); });

    std::cout << "Net thread created" << std::endl;

    tetrisClient->Connect();

    //std::cout << "Connected" << std::endl;

    while (tetrisClient->run)
    {
        //std::cout << "Running" << std::endl;
        tetrisClient->Run();
    }

    return 0;
}