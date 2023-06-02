#include "Tetris.cc"

int main(int argc, char **argv)
{    
    TetrisServer *tetrisServer = new TetrisServer(argv[1], argv[2]);

    tetrisServer->do_messages();

    return 0;
}