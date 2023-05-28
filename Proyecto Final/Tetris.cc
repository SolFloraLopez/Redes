#include "Tetris.h"
#include <iostream>
#include <stdlib.h>

void TetrisClient::DrawGrid(int offsetX, int offsetY, TetrisBoard b)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i <= b.HEIGHT; i++)
    {
        int x = offsetX + b.TILE_SIZE * b.LENGTH;
        int y = offsetY + b.TILE_SIZE * i;

        SDL_RenderDrawLine(renderer, offsetX, y, x, y);
    }

    for (int i = 0; i <= b.LENGTH; i++)
    {
        int x = offsetX + b.TILE_SIZE * i;
        int y = offsetY + b.TILE_SIZE * b.HEIGHT;
        ;

        SDL_RenderDrawLine(renderer, x, offsetY, x, y);
    }

    SDL_Rect nextPieceRect;

    nextPieceRect.x = offsetX + b.TILE_SIZE * b.LENGTH;
    nextPieceRect.y = offsetY;
    nextPieceRect.w = b.TILE_SIZE * (b.HEIGHT / 8);
    nextPieceRect.h = b.TILE_SIZE * (b.HEIGHT / 8);

    SDL_RenderDrawRect(renderer, &nextPieceRect);
}

void TetrisClient::DrawTiles(int offsetX, int offsetY, TetrisBoard b)
{
    for (int i = 0; i < b.HEIGHT; i++)
    {
        for (int j = 0; j < b.LENGTH; j++)
        {
            int tile = i * b.LENGTH + j;
            uint8_t tileColor = b.tiles[tile];

            if (tileColor != TetrisBoard::TileType::EMPTY)
            {
                switch (tileColor)
                {
                case TetrisBoard::TileType::RED:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    break;
                case TetrisBoard::TileType::ORANGE:
                    SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
                    break;
                case TetrisBoard::TileType::YELLOW:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                    break;
                case TetrisBoard::TileType::GREEN:
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    break;
                case TetrisBoard::TileType::BLUE:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    break;
                case TetrisBoard::TileType::PURPLE:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                    break;
                case TetrisBoard::TileType::PINK:
                    SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255);
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    break;
                }

                SDL_Rect tileRect;
                tileRect.x = offsetX + j * b.TILE_SIZE + 1;
                tileRect.y = offsetY + i * b.TILE_SIZE + 1;
                tileRect.w = b.TILE_SIZE - 1;
                tileRect.h = b.TILE_SIZE - 1;

                SDL_RenderFillRect(renderer, &tileRect);
            }
        }
    }
}

void TetrisClient::DrawBoard(int offsetX, int offsetY, TetrisBoard b)
{
    DrawGrid(offsetX, offsetY, b);
    DrawTiles(offsetX, offsetY, b);
}

bool TetrisClient::CheckBoundsRight()
{
    for (int i = 0; i < 4; i++)
        if ((positions[i] + 1) % board1.LENGTH == 0)
            return false;
    return true;
}

bool TetrisClient::CheckBoundsLeft()
{
    for (int i = 0; i < 4; i++)
        if ((positions[i] - 1) % board1.LENGTH == board1.LENGTH - 1)
            return false;
    return true;
}

bool TetrisClient::CheckBoundsDown()
{
    for (int i = 0; i < 4; i++)
        if ((positions[i] + board1.LENGTH) >= board1.HEIGHT * board1.LENGTH)
            return false;
    return true;
}

bool TetrisClient::CheckColissions()
{
    for (int i = 0; i < 4; i++)
        if (board1.tiles[positions[i]] != TetrisBoard::TileType::EMPTY) 
            return false;
    return true;
}

void TetrisClient::Init()
{
    srand(time(NULL));
    currentTetramino = new Tetramino(2);

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "SDL could not be initialized: " << SDL_GetError();
    }

    else
        std::cout << "SDL video system initialized" << std::endl;

    window = SDL_CreateWindow("TetrisVS", 0, 0, 900, 900, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr)
        std::cout << "Problem creating Renderer" << std::endl;

    positions[0] = (board1.HEIGHT / 2) * board1.LENGTH + board1.LENGTH / 2;

    lastPivotPosition = positions[0];
    lastRotation = currentTetramino->rotation;

    for (int i = board1.HEIGHT * board1.LENGTH - 1; i > 300; i--)
        board1.tiles[i] = rand() % 2;

    run = true;
    gettimeofday(&initialTime, NULL);
}

void TetrisClient::Render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    DrawBoard(100, 100 - 2 * board1.TILE_SIZE, board1);
    DrawBoard(550, 100 - 2 * board2.TILE_SIZE, board2);

    SDL_RenderPresent(renderer);
}

void TetrisClient::ProcessInput(SDL_Event event)
{
    switch (event.key.keysym.sym)
            {
            case SDLK_RIGHT:
                if (CheckBoundsRight())
                {
                    lastPivotPosition = positions[0];
                    positions[0]++;
                }
                break;
            case SDLK_LEFT:
                if (CheckBoundsLeft())
                {
                    lastPivotPosition = positions[0];
                    positions[0]--;
                }
                break;
            case SDLK_DOWN:
                if (CheckBoundsDown())
                {
                    lastPivotPosition = positions[0];
                    positions[0] += board1.LENGTH;
                }
                break;
            case SDLK_q:
                if (CheckBoundsDown() && CheckBoundsLeft() && CheckBoundsDown())
                {
                    lastRotation = currentTetramino->rotation;
                    currentTetramino->Rotate(-1);
                }
                break;
            case SDLK_e:
                if (CheckBoundsDown() && CheckBoundsLeft() && CheckBoundsDown())
                {
                    lastRotation = currentTetramino->rotation;
                    currentTetramino->Rotate(1);
                }
                break;
            default:
                break;
            }

            if (lastPivotPosition != positions[0] || lastRotation != currentTetramino->rotation)
            {
                currentTetramino->GetPiecePositions(board1.LENGTH, positions);

                if (!CheckColissions())
                {
                    positions[0] = lastPivotPosition;
                    currentTetramino->Rotate(lastRotation - currentTetramino->rotation);
                    currentTetramino->GetPiecePositions(board1.LENGTH, positions);
                }

                //std::cout << valid << std::endl;
            }
}

void TetrisClient::Run()
{
    gettimeofday(&elapsedTime, NULL);
    int seconds = elapsedTime.tv_sec - initialTime.tv_sec;

    currentTetramino->GetPiecePositions(board1.LENGTH, positions);
    for (int i = 0; i < 4; i++)
        board1.tiles[positions[i]] = TetrisBoard::TileType::EMPTY;
    //std::cout << seconds << ":" << elapsedTime.tv_usec<< std::endl;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            run = false;
            break;
        case SDL_KEYDOWN:
            ProcessInput(event);  
            break;  
        default:
            break;
        }
    }

    for (int i = 0; i < 4; i++)
        board1.tiles[positions[i]] = currentTetramino->_id + 1;



    for (int i = 0; i < board1.HEIGHT; i++)
    {
        for (int j = 0; j < board1.LENGTH; j++)
        {
            int tile = i * board1.LENGTH + j;

            //std::cout << unsigned(board1.tiles[tile]) << " ";
        }

        //std::cout << std::endl;
    }

    

    //currentTetramino->Rotate(1);
    //std::cout << currentTetramino->rotation << std::endl;
    //currentTetramino->_id = (currentTetramino->_id + 1) % 7;
    //currentTetramino = new currentTetramino(currentTetramino->_id);

    //positions[0] += board1.LENGTH;

    Render();
    SDL_Delay(1000 / 60);
}

int main(int argc, char **argv)
{
    TetrisClient* tetrisClient = new TetrisClient();
    tetrisClient->Init();

    while (tetrisClient->run)
    {
        tetrisClient->Run();
    }

    delete tetrisClient;

    return 0;
}