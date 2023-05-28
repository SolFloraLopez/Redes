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
                SetToTileColor(tileColor);

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

void TetrisClient::DrawNextPiece(int offsetX, int offsetY, TetrisBoard b)
{
    Tetramino *refTetramino = new Tetramino(b.nextTetraminoID);

    int xCenter = offsetX + b.TILE_SIZE * b.LENGTH + b.TILE_SIZE * (b.HEIGHT / 16);
    int yCenter = offsetY + b.TILE_SIZE * (b.HEIGHT / 16);

    for (int i = 0; i < 4; i++)
    {
        int xPos;
        int yPos;

        if (i == 0)
        {
            xPos = 0; 
            yPos = 0;
        } 
        
        else
        {
            xPos = refTetramino->initialPos[i].first;
            yPos = refTetramino->initialPos[i].second;
        }

        SDL_Rect tileRect;
        tileRect.x = xCenter + xPos * b.TILE_SIZE;
        tileRect.y = yCenter + yPos * b.TILE_SIZE;
        tileRect.w = b.TILE_SIZE - 1;
        tileRect.h = b.TILE_SIZE - 1;

        SetToTileColor(refTetramino->_id + 1);
        SDL_RenderFillRect(renderer, &tileRect);

        tileRect.w = b.TILE_SIZE;
        tileRect.h = b.TILE_SIZE;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &tileRect);
    }
}

void TetrisClient::SetToTileColor(uint8_t id)
{
    switch (id)
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
}

void TetrisClient::DrawBoard(int offsetX, int offsetY, TetrisBoard b)
{
    DrawGrid(offsetX, offsetY, b);
    DrawTiles(offsetX, offsetY, b);
    DrawNextPiece(offsetX, offsetY, b);
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

bool TetrisClient::CorrectRotation()
{
    currentTetramino->GetPiecePositions(board1.LENGTH, positions);

    if (!CheckColissions())
    {
        positions[0] -= board1.LENGTH;

        if (!CorrectPosition())
        {
            currentTetramino->Rotate(lastRotation - currentTetramino->rotation);
            currentTetramino->GetPiecePositions(board1.LENGTH, positions);

            return false;
        }
    }

    return true;
}

bool TetrisClient::CorrectPosition()
{
    currentTetramino->GetPiecePositions(board1.LENGTH, positions);

    if (!CheckColissions())
    {
        positions[0] = lastPivotPosition;
        currentTetramino->GetPiecePositions(board1.LENGTH, positions);

        return false;
    }

    return true;
}

void TetrisClient::SetPiece()
{
    CheckRows();
    ChangePiece();
    positions[0] = board1.LENGTH + board1.LENGTH / 2;
}

void TetrisClient::ChangePiece()
{
    currentTetramino = new Tetramino(board1.nextTetraminoID);

    if (tetraminoPool.size() <= 0)
        GeneratePieces();

    int next = rand() % tetraminoPool.size();
    board1.nextTetraminoID = tetraminoPool[next];
    tetraminoPool.erase(tetraminoPool.begin() + next);
}

void TetrisClient::GeneratePieces()
{
    for (uint8_t i = 0; i < 7; i++)
        tetraminoPool.emplace_back(i);
}

void TetrisClient::CheckRows()
{
    int consecutiveRows = 0;

    for (int i = 0; i < board1.HEIGHT; i++)
    {
        int j = 0;
        bool foundEmpty = false;

        while(j < board1.LENGTH && !foundEmpty)
        {
            if(board1.tiles[i * board1.LENGTH + j] == TetrisBoard::EMPTY) foundEmpty = true;
            j++;
        }

        if(!foundEmpty) 
        {
            std::cout << "Deleted!" << std::endl;

            consecutiveRows++;
            if(consecutiveRows == 1) firstRow = i;
            for(int h = 0; h < board1.LENGTH; h++) board1.tiles[i * board1.LENGTH + h] = TetrisBoard::EMPTY;
            rowsDeleted++;
        }

        else if(consecutiveRows > 0)
        {
            std::cout << "updated!" << std::endl;
            UpdateRows(consecutiveRows);
            consecutiveRows = 0;
        }
    }

    if (consecutiveRows > 0)
    {
        std::cout << "updated!" << std::endl;
        UpdateRows(consecutiveRows);
        consecutiveRows = 0;
    }

    std::cout << std::endl;
}

void TetrisClient::UpdateRows(int rows)
{
    std::cout << firstRow << std::endl;
    std::cout << firstRow * board1.LENGTH - 1 << std::endl;

    for (int i = firstRow * board1.LENGTH - 1; i >= 0; i--)
    {
        board1.tiles[i + rows * board1.LENGTH] = board1.tiles[i];
        board1.tiles[i] = TetrisBoard::EMPTY;
    }
}

void TetrisClient::Init()
{
    srand(time(NULL));
    GeneratePieces();
    ChangePiece();

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

    // for (int i = board1.HEIGHT * board1.LENGTH - 1; i > 300; i--)
    //     board1.tiles[i] = rand() % 2;

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
            if (!CorrectPosition())
                set = true;
        }
        break;
    case SDLK_LEFT:
        if (CheckBoundsLeft())
        {
            lastPivotPosition = positions[0];
            positions[0]--;
            if (!CorrectPosition())
                set = true;
        }
        break;
    case SDLK_DOWN:
        if (CheckBoundsDown())
        {
            lastPivotPosition = positions[0];
            positions[0] += board1.LENGTH;
            if (!CorrectPosition())
                set = true;
        }
        else
            set = true;
        break;
    case SDLK_q:
        if (CheckBoundsDown() && CheckBoundsLeft() && CheckBoundsDown())
        {
            lastRotation = currentTetramino->rotation;
            currentTetramino->Rotate(-1);
            if (!CorrectRotation())
                set = true;
        }
        break;
    case SDLK_e:
        if (CheckBoundsDown() && CheckBoundsLeft() && CheckBoundsDown())
        {
            lastRotation = currentTetramino->rotation;
            currentTetramino->Rotate(1);
            if (!CorrectRotation())
                set = true;
        }
        break;
    default:
        break;
    }
}

void TetrisClient::Run()
{
    remainingFrames--;

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

    if (remainingFrames <= 0)
    {
        if (CheckBoundsDown())
        {
            lastPivotPosition = positions[0];
            positions[0] += board1.LENGTH;
            if (!CorrectPosition()) set = true;
        }
        else set = true;

        remainingFrames = framesPerTick;
        remainingTicks--;

        if(remainingTicks <= 0) 
        {
            framesPerTick--;
            remainingTicks = ticksToSpeedUp;
            //std::cout << framesPerTick << std::endl;
        }
    }

    for (int i = 0; i < 4; i++)
        board1.tiles[positions[i]] = currentTetramino->_id + 1;

    if (set)
    {
        SetPiece();
        set = false;
    }

    rowsDeleted = 0;

    // for (int i = 0; i < board1.HEIGHT; i++)
    // {
    //     for (int j = 0; j < board1.LENGTH; j++)
    //     {
    //         int tile = i * board1.LENGTH + j;

    //         //std::cout << unsigned(board1.tiles[tile]) << " ";
    //     }

    //     //std::cout << std::endl;
    // }

    Render();

    SDL_Delay(1000 / 60);
}

int main(int argc, char **argv)
{
    TetrisClient *tetrisClient = new TetrisClient();
    tetrisClient->Init();

    while (tetrisClient->run)
    {
        tetrisClient->Run();
    }

    delete tetrisClient;

    return 0;
}