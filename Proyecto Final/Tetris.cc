#include "Tetris.h"
#include <iostream>
#include <stdlib.h>

void Message::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* aux = _data;

    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);

    memcpy(aux, static_cast<const void*>(&set), sizeof(int));
    aux += sizeof(int);

    memcpy(aux, static_cast<const void*>(&currentTetraminoID), sizeof(int));
    aux += sizeof(int);

    memcpy(aux, static_cast<const void*>(&pivotPos), sizeof(int));
    aux += sizeof(int);

    memcpy(aux, static_cast<const void*>(&rotation), sizeof(int));
    aux += sizeof(int);

    memcpy(aux, playerName.c_str(), sizeof(char) * 8);
    aux += sizeof(char) * 8;
}

int Message::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* aux = _data;

    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);

    memcpy(&set, aux, sizeof(int));
    aux += sizeof(int);

    memcpy(&currentTetraminoID, aux, sizeof(int));
    aux += sizeof(int);

    memcpy(&pivotPos, aux, sizeof(int));
    aux += sizeof(int);

    memcpy(&rotation, aux, sizeof(int));
    aux += sizeof(int);

    playerName = std::string(aux, aux + sizeof(char) * 8);
    aux += sizeof(char) * 8;

    return 0;
}

// ##CORE GAME LOOP FUCNTIONS##
void TetrisClient::Init()
{
    srand(time(NULL));

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

    else std::cout << "Renderer created" << std::endl;

    board1 = new TetrisBoard();
    board2 = new TetrisBoard();

    run = true;
    gettimeofday(&initialTime, NULL);
}

void TetrisClient::Render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    board1->DrawBoard(100, 100 - 2 * board1->TILE_SIZE, true, renderer);
    board2->DrawBoard(550, 100 - 2 * board2->TILE_SIZE, false, renderer);

    SDL_RenderPresent(renderer);
}

void TetrisClient::ProcessInput(SDL_Event event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_RIGHT:
        if (board1->CheckBoundsRight())
        {
            board1->lastPivotPosition = board1->positions[0];
            board1->positions[0]++;
            board1->CorrectPosition();
            SendData(set);
        }
        break;
    case SDLK_LEFT:
        if (board1->CheckBoundsLeft())
        {
            board1->lastPivotPosition = board1->positions[0];
            board1->positions[0]--;
            board1->CorrectPosition();
            SendData(set);
        }
        break;
    case SDLK_DOWN:
        if (board1->CheckBoundsDown())
        {
            board1->lastPivotPosition = board1->positions[0];
            board1->positions[0] += board1->LENGTH;
            if (!board1->CorrectPosition())
                board1->set = true;
            SendData(set);
        }
        else
            board1->set = true;
        break;
    case SDLK_q:
        board1->Rotate(-1);
        SendData(set);
        break;
    case SDLK_e:
        board1->Rotate(1);
        SendData(set);
        break;
    default:
        break;
    }
}

void TetrisClient::ProcessMsg()
{
    if(lastMsg.type == Message::MessageType::DATA && !processed)
    {
        if(board2->currentTetramino->_id != lastMsg.currentTetraminoID) board2->currentTetramino = new Tetramino(lastMsg.currentTetraminoID);

        board2->positions[0] = lastMsg.pivotPos;
        board2->currentTetramino->Rotate(lastMsg.rotation - board2->currentTetramino->rotation);

        board2->currentTetramino->GetPiecePositions(board2->LENGTH, board2->positions);
        board2->set = lastMsg.set == 1;

        processed = true;
    }
}

void TetrisClient::Run()
{
    input = false;

    SDL_Event event;
    SDL_Event keyEvent;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            gameState == DISCONNECT;
            Disconnect();
            run = false;
            break;
        case SDL_KEYDOWN:
            if(PLAYING && event.key.keysym.sym > 0)
            {
                input = true;
                keyEvent.key.keysym.sym = event.key.keysym.sym;
            }
            break;
        default:
            break;
        }
    }

    if(gameState == PLAYING) 
    {
        remainingFrames--;

        gettimeofday(&elapsedTime, NULL);
        int seconds = elapsedTime.tv_sec - initialTime.tv_sec;

        board1->ClearTetramino();
        board2->ClearTetramino();

        ProcessMsg();

        if(input) ProcessInput(keyEvent);

        if (remainingFrames <= 0)
        {
            if (board1->CheckBoundsDown()) board1->set = board1->CheckPieceSets();
            else board1->set = true;
            SendData(set);

            remainingTicks--;
        }

        board1->Update();
        board2->Update();

        if (board1->set)
        {
            SendData(true);
            if(!board1->SetPiece()) EndGame();
            board1->set = false;
        }

        if (board2->set)
        {   
            board2->SetPiece();
            board2->set = false;
        }

        if (remainingTicks <= 0)
        {
            framesPerTick--;
            remainingTicks = ticksToSpeedUp;
            if (framesPerTick < minFramesPerTick) framesPerTick = minFramesPerTick;
        }   

        if (remainingFrames <= 0)
        {
            //Sendmsg();
            remainingFrames = framesPerTick;
        }

        SDL_Delay(1000 / 60);
    }

    if(gameState == RESULT) 
    {
        char response;

        std::cin >> response;
        
        if(response == 'Y') Restart();
        else if(response == 'N') Disconnect();
        else
        {
            std::cout << "PLAY AGAIN? (Y/N): ";
        }
    }

    if(gameState == DISCONNECT)
    {
        Disconnect();
        run = false;
    }

    Render();
}

void TetrisClient::EndGame()
{
    gameState = RESULT;

    Message msg;
    msg.type = Message::MessageType::END;
    msg.playerName = playerName;

    socket.send(msg, socket);

    std::cout << "YOU LOST" << std::endl;
    std::cout << "PLAY AGAIN? (Y/N): ";
}

void TetrisClient::Restart()
{
    board1 = new TetrisBoard();
    board2 = new TetrisBoard();

    if(lastMsg.type == Message::MessageType::START)
    {
        gameState = PLAYING;
        SendData(set);
        std::cout << "PLAYING AGAINST: " << lastMsg.playerName << std::endl;

        Message msg;

        msg.type = Message::MessageType::START;
        msg.playerName = playerName;

        socket.send(msg, socket);
    }

    else
    {
        gameState = WAITING;

        Message msg;

        msg.type = Message::MessageType::START;
        msg.playerName = playerName;

        socket.send(msg, socket);

        std::cout << "WAITING FOR " << lastMsg.playerName << std::endl;
    }
}

void TetrisClient::Connect()
{
    std::cout << "CONNECTING" << std::endl;

    Message msg;
    msg.type = Message::MessageType::CONNECT;
    msg.playerName = playerName;

    socket.send(msg, socket);

    gameState = WAITING;
    std::cout << "WAITING FOR ANOTHER PLAYER" << std::endl;
}

void TetrisClient::Disconnect()
{
    std::cout << "DISCONNECTING" << std::endl;

    Message msg;
    msg.type = Message::MessageType::DISCONNECT;
    msg.playerName = playerName;

    socket.send(msg, socket);
}

void TetrisClient::SendData(bool s)
{
    Message msg;
    msg.type = Message::MessageType::DATA;
    msg.playerName = playerName;
    msg.set = s;
    msg.currentTetraminoID = board1->currentTetramino->_id;
    
    msg.pivotPos = board1->positions[0];
    msg.rotation = board1->currentTetramino->rotation;

    socket.send(msg, socket);
}

void TetrisClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        Message msg;

        if(socket.recv(msg) >= 0)
        {
            lastMsg = msg;
            processed = false;

            switch(msg.type)
            {
            case Message::MessageType::START:
                if(gameState = WAITING)
                {
                    gameState = PLAYING;
                    SendData(set);
                    std::cout << "PLAYING AGAINST: " << msg.playerName << std::endl;
                }
                break;
            case Message::MessageType::END:
                gameState = RESULT;
                std::cout << "YOU WON" << std::endl;
                std::cout << "PLAY AGAIN? (Y/N): ";
            default:
                break;
            }
        }
    }
}

void TetrisServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        
        Message msg;
	    Socket *msgSocket;
        
        int res = socket.recv(msg, msgSocket);

        std::unique_ptr<Socket> client(msgSocket);

        if (msg.type == Message::MessageType::CONNECT){    
            std::cout << "CONNECTING WITH " << msg.playerName << std::endl;                    
            if(clients.size() < 2)
            {
                clients.push_back(std::move(client));
                names.push_back(msg.playerName);
            }

            if (clients.size() >= 2 && state == CONNECTING) 
            {
                std::cout << "LOBBY FULL" << std::endl;

                Message startMsg;
                startMsg.type = Message::MessageType::START;

                int i = names.size() - 1;

                for(auto it = clients.begin(); it != clients.end(); ++it)
                {
                    startMsg.playerName = names[i];
                    socket.send(startMsg, **it);
                    i--;
                }

                state = PLAYING;
            }
        }

        else if((state != CONNECTING && state != DISCONNECT) && (msg.type > 0 && msg.type < 4)){
            for(auto it = clients.begin(); it != clients.end(); ++it) if(!(**it == *client)) socket.send(msg, **it);

            if(msg.type == Message::MessageType::END) state == RESULT;
        }

        else if(msg.type == Message::MessageType::DISCONNECT){
            std::cout << "DISCONNECTING " << msg.playerName << std::endl;
            auto it = clients.begin();
            auto nameIt = names.begin();
            while(it != clients.end() && !(**it == *client)) 
            {
                it++; 
                nameIt++;
            }
            if(it != clients.end())
            {
                clients.erase(it);
                names.erase(nameIt);
            }
        }

        else std::cerr << "ERROR" << std::endl;
    }
}