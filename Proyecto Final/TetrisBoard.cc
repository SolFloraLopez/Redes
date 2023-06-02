#include "TetrisBoard.h"

// ##RENDER UTILS##
void TetrisBoard::DrawGrid(int offsetX, int offsetY, bool drawNext, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i <= HEIGHT; i++)
    {
        int x = offsetX + TILE_SIZE * LENGTH;
        int y = offsetY + TILE_SIZE * i;
        SDL_RenderDrawLine(renderer, offsetX, y, x, y);
    }

    for (int i = 0; i <= LENGTH; i++)
    {
        int x = offsetX + TILE_SIZE * i;
        int y = offsetY + TILE_SIZE * HEIGHT;

        SDL_RenderDrawLine(renderer, x, offsetY, x, y);
    }


    if(drawNext)
    {
        SDL_Rect nextPieceRect;

        nextPieceRect.x = offsetX + TILE_SIZE * LENGTH;
        nextPieceRect.y = offsetY;
        nextPieceRect.w = TILE_SIZE * (HEIGHT / 8);
        nextPieceRect.h = TILE_SIZE * (HEIGHT / 8);

        SDL_RenderDrawRect(renderer, &nextPieceRect);
    }
}

void TetrisBoard::DrawTiles(int offsetX, int offsetY, SDL_Renderer* renderer)
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < LENGTH; j++)
        {
            int tile = i * LENGTH + j;
            uint8_t tileColor = tiles[tile];

            if (tileColor != EMPTY)
            {
                SetToTileColor(tileColor, renderer);

                SDL_Rect tileRect;
                tileRect.x = offsetX + j * TILE_SIZE + 1;
                tileRect.y = offsetY + i * TILE_SIZE + 1;
                tileRect.w = TILE_SIZE - 1;
                tileRect.h = TILE_SIZE - 1;

                SDL_RenderFillRect(renderer, &tileRect);
            }
        }
    }
}

void TetrisBoard::DrawNextPiece(int offsetX, int offsetY, SDL_Renderer* renderer)
{
    Tetramino *refTetramino = new Tetramino(nextTetraminoID);

    int xCenter = offsetX + TILE_SIZE * LENGTH + TILE_SIZE * (HEIGHT / 16);
    int yCenter = offsetY + TILE_SIZE * (HEIGHT / 16);

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
        tileRect.x = xCenter + xPos * TILE_SIZE;
        tileRect.y = yCenter + yPos * TILE_SIZE;
        tileRect.w = TILE_SIZE - 1;
        tileRect.h = TILE_SIZE - 1;

        SetToTileColor(refTetramino->_id + 1, renderer);
        SDL_RenderFillRect(renderer, &tileRect);

        tileRect.w = TILE_SIZE;
        tileRect.h = TILE_SIZE;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &tileRect);
    }
}

void TetrisBoard::SetToTileColor(uint8_t id, SDL_Renderer* renderer)
{
    switch (id)
    {
    case RED:
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        break;
    case ORANGE:
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
        break;
    case YELLOW:
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        break;
    case GREEN:
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        break;
    case BLUE:
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        break;
    case PURPLE:
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        break;
    case PINK:
        SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255);
        break;
    default:
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        break;
    }
}

void TetrisBoard::DrawBoard(int offsetX, int offsetY, bool drawNext, SDL_Renderer* renderer)
{
    DrawGrid(offsetX, offsetY, drawNext, renderer);
    DrawTiles(offsetX, offsetY, renderer);
    if(drawNext) DrawNextPiece(offsetX, offsetY, renderer);
}

// ##COLLISION UTILS##
bool TetrisBoard::CheckBoundsRight()
{
    for (int i = 0; i < 4; i++)
        if ((positions[i] + 1) % LENGTH == 0)
            return false;
    return true;
}

bool TetrisBoard::CheckBoundsLeft()
{
    for (int i = 0; i < 4; i++)
        if ((positions[i] - 1) % LENGTH == LENGTH - 1)
            return false;
    return true;
}

bool TetrisBoard::CheckBoundsDown()
{
    for (int i = 0; i < 4; i++)
        if ((positions[i] + LENGTH) >= HEIGHT * LENGTH)
            return false;
    return true;
}

bool TetrisBoard::CheckColissions()
{
    for (int i = 0; i < 4; i++)
        if (tiles[positions[i]] != EMPTY)
            return false;
    return true;
}

int TetrisBoard::CheckDistance(int piece)
{
    if(positions[piece] / LENGTH != positions[0] / LENGTH) 
    {
        return positions[piece] % LENGTH - positions[0] % LENGTH;
    }

    else return 0;
}

bool TetrisBoard::Rotate(int dir)
{
    lastRotation = currentTetramino->rotation;

    currentTetramino->Rotate(1);
    currentTetramino->GetPiecePositions(LENGTH, positions);

    for(int i = 1; i <= 3; i++)
    {
        int dist = CheckDistance(i);

        if (dist > 1) positions[0]++;
        else if(dist < -1) positions[0]--;
    }

    currentTetramino->GetPiecePositions(LENGTH, positions);

    if (!CheckBoundsDown() || !CheckColissions()) positions[0] -= LENGTH;

    if (!CorrectPosition())
    {
        currentTetramino->Rotate(lastRotation - currentTetramino->rotation);
        currentTetramino->GetPiecePositions(LENGTH, positions);

        return false;
    }

    return true;
}

bool TetrisBoard::CorrectPosition()
{
    currentTetramino->GetPiecePositions(LENGTH, positions);

    if (!CheckColissions())
    {
        positions[0] = lastPivotPosition;
        currentTetramino->GetPiecePositions(LENGTH, positions);

        return false;
    }

    return true;
}

// ##PIECE PLACEMENT AND GENERATION##
bool TetrisBoard::CheckPieceSets()
{
    lastPivotPosition = positions[0];
    positions[0] += LENGTH;

    return !CorrectPosition();
}

bool TetrisBoard::SetPiece()
{
    bool run = true;
    currentTetramino->GetPiecePositions(LENGTH, positions);

    if(!CheckRows() && positions[0] < LENGTH) run = false;
    ChangePiece();
    positions[0] = LENGTH / 2;

    return run;
}

void TetrisBoard::ChangePiece()
{
    currentTetramino = new Tetramino(nextTetraminoID);

    if (tetraminoPool.size() <= 0)
        GeneratePieces();

    int next = rand() % tetraminoPool.size();
    nextTetraminoID = tetraminoPool[next];
    tetraminoPool.erase(tetraminoPool.begin() + next);
}

void TetrisBoard::GeneratePieces()
{
    for (uint8_t i = 0; i < 7; i++)
        tetraminoPool.emplace_back(i);
}

bool TetrisBoard::CheckRows()
{
    int consecutiveRows = 0;

    for (int i = 0; i < HEIGHT; i++)
    {
        int j = 0;
        bool foundEmpty = false;

        while(j < LENGTH && !foundEmpty)
        {
            if(tiles[i * LENGTH + j] == EMPTY) foundEmpty = true;
            j++;
        }

        if(!foundEmpty) 
        {
            consecutiveRows++;
            if(consecutiveRows == 1) firstRow = i;
            for(int h = 0; h < LENGTH; h++) tiles[i * LENGTH + h] = EMPTY;
            rowsDeleted++;
        }

        else if(consecutiveRows > 0)
        {
            UpdateRows(consecutiveRows);
            consecutiveRows = 0;
        }
    }

    if (consecutiveRows > 0)
    {
        UpdateRows(consecutiveRows);
        consecutiveRows = 0;
    }

    return rowsDeleted > 0;
}

void TetrisBoard::UpdateRows(int rows)
{
    for (int i = firstRow * LENGTH - 1; i >= 0; i--)
    {
        tiles[i + rows * LENGTH] = tiles[i];
        tiles[i] = EMPTY;
    }
}

void TetrisBoard::ClearTetramino()
{
    currentTetramino->GetPiecePositions(LENGTH, positions);

    for (int i = 0; i < 4; i++) if(positions[i] > 0) tiles[positions[i]] = EMPTY;
}

void TetrisBoard::Update()
{
    for (int i = 0; i < 4; i++) if(positions[i] > 0) tiles[positions[i]] = currentTetramino->_id + 1;

    rowsDeleted = 0;
}