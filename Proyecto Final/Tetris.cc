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
    cout << aux << std::endl;

    memcpy(aux, &input, sizeof(char));
    aux += sizeof(char);

    memcpy(aux, static_cast<const void*>(&nextTetraminoID), sizeof(int));
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

    memcpy(&input, aux, sizeof(char));
    aux += sizeof(char);

    memcpy(&nextTetraminoID, aux, sizeof(int));
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

    std::cout << "msg Initialized" << std::cout;

    run = true;
    gettimeofday(&initialTime, NULL);
}

void TetrisClient::Render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //std::cout << "Calling Drawmsg()" << std::cout;
    board1->DrawBoard(100, 100 - 2 * board1->TILE_SIZE, renderer);
    board2->DrawBoard(550, 100 - 2 * board2->TILE_SIZE, renderer);

    //std::cout << "Calling RenderPresent()" << std::cout;
    SDL_RenderPresent(renderer);
}

void TetrisClient::ProcessInput(SDL_Event event)
{
    //std::cout << event.key.keysym.sym << std::endl;

    switch (event.key.keysym.sym)
    {
    case SDLK_RIGHT:
        if (board1->CheckBoundsRight())
        {
            board1->lastPivotPosition = board1->positions[0];
            board1->positions[0]++;
            board1->CorrectPosition();
            SendInput('R');
        }
        break;
    case SDLK_LEFT:
        if (board1->CheckBoundsLeft())
        {
            board1->lastPivotPosition = board1->positions[0];
            board1->positions[0]--;
            board1->CorrectPosition();
            SendInput('L');
        }
        break;
    case SDLK_DOWN:
        if (board1->CheckBoundsDown())
        {
            board1->lastPivotPosition = board1->positions[0];
            board1->positions[0] += board1->LENGTH;
            if (!board1->CorrectPosition())
                board1->set = true;
            SendInput('D');
        }
        else
            board1->set = true;
        break;
    case SDLK_q:
        board1->Rotate(-1);
        SendInput('Q');
        break;
    case SDLK_e:
        board1->Rotate(1);
        SendInput('E');
        break;
    default:
        break;
    }
}

void TetrisClient::ProcessInputMsg()
{
    switch (inputMsg)
    {
    // case 'R':
    //     if (board2->CheckBoundsRight())
    //     {
    //         board2->lastPivotPosition = board2->positions[0];
    //         board2->positions[0]++;
    //         board2->CorrectPosition();
    //     }
    //     break;
    // case 'L':
    //     if (board2->CheckBoundsLeft())
    //     {
    //         board2->lastPivotPosition = board2->positions[0];
    //         board2->positions[0]--;
    //         board2->CorrectPosition();
    //     }
    //     break;
    // case 'D':
    //     if (board2->CheckBoundsDown())
    //     {
    //         board2->lastPivotPosition = board2->positions[0];
    //         board2->positions[0] += board2->LENGTH;
    //         board2->CorrectPosition();
    //     }
    //     break;
    // case 'Q':
    //     board2->Rotate(-1);
    //     break;
    // case 'E':
    //     board2->Rotate(1);
    //     break;
    case 'S':
        board2->set = true;
    default:
        break;
    }

    inputMsg = ' ';
}

void TetrisClient::Run()
{
    //std::cout << gameState << std::endl;

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

        std::cout << "Board1: " << std::endl;
        board1->ClearTetramino();
        board2->ClearTetramino();

        //std::cout << seconds << ":" << elapsedTime.tv_usec<< std::endl;
        if(input) ProcessInput(keyEvent);
        ProcessInputMsg();

        if (remainingFrames <= 0)
        {
            if (board1->CheckBoundsDown()) board1->set = board1->CheckPieceSets();
            else board1->set = true;
            SendInput(' ');

            //if (board2->CheckBoundsDown()) board2->CheckPieceSets();

            remainingTicks--;
        }

        board1->Update();
        board2->Update();

        if (board1->set)
        {
            SendInput('S');
            //std::cout << "Calling SetPiece()" << std::cout;
            if(!board1->SetPiece()) gameState = RESULT;
            board1->set = false;
        }

        if (board2->set)
        {   
            board2->SetPiece();
            board2->set = false;
        }

        //std::cout << "Calling Render()" << std::cout;

        if (remainingTicks <= 0)
        {
            framesPerTick--;
            remainingTicks = ticksToSpeedUp;
            if (framesPerTick < minFramesPerTick) framesPerTick = minFramesPerTick;

            //std::cout << "Calling Sendmsg()" << std::cout;
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

    }

    if(gameState == DISCONNECT)
    {
        Disconnect();
        run = false;
    }

    Render();
}

void TetrisClient::Connect()
{
    std::cout << "CONNECTING" << std::endl;

    Message msg;
    msg.type = Message::MessageType::CONNECT;
    msg.playerName = playerName;
    msg.nextTetraminoID = board1->nextTetraminoID;

    socket.send(msg, socket);

    gameState = WAITING;
}

void TetrisClient::Disconnect()
{
    std::cout << "DISCONNECTING" << std::endl;

    Message msg;
    msg.type = Message::MessageType::DISCONNECT;
    msg.playerName = playerName;

    socket.send(msg, socket);
}

void TetrisClient::SendInput(char i)
{
    std::cout << "SENDING INPUT: " << i << std::endl;

    Message msg;
    msg.type = Message::MessageType::INPUT;
    msg.nextTetraminoID = board1->nextTetraminoID;
    msg.playerName = playerName;
    msg.input = i;
    
    msg.pivotPos = board1->positions[0];
    msg.rotation = board1->currentTetramino->rotation;

    std::cout << msg.pivotPos << std::endl;
    std::cout << msg.rotation << std::endl;

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
            switch(msg.type)
            {
            case Message::MessageType::START:
                gameState = PLAYING;
                SendInput(' ');
                //std::cout << "PLAYING AGAINST: " << msg.playerName << std::endl;
                break;
            case Message::MessageType::INPUT:
                if(gameState == PLAYING) 
                {
                    inputMsg = msg.input;
                    board2->nextTetraminoID = msg.nextTetraminoID;

                    std::cout << "Position: " << msg.pivotPos << std::endl;
                    std::cout << "Rotation: " << msg.rotation << std::endl;

                    std::cout << "Board2: " << std::endl;
                    board2->ClearTetramino();
                    
                    board2->positions[0] = msg.pivotPos;
                    board2->currentTetramino->Rotate(msg.rotation - board2->currentTetramino->rotation);

                    board2->currentTetramino->GetPiecePositions(board2->LENGTH, board2->positions);
                    board2->Update();
                }
                break;
            case Message::MessageType::END:
                gameState = RESULT;
            default:
                break;
            }

            //std::cout << gameState << std::endl;
        }

        
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        //std::cout << msg.nick << ": " << msg.message << std::endl;
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

        //if(res < 0) continue;

        std::unique_ptr<Socket> client(msgSocket);

        std::cout << "NAME: " << msg.playerName << " TYPE: " << unsigned(msg.type) /*<< "MESSAGGE: " << msg.type*/ << std::endl;

        if (msg.type == Message::MessageType::CONNECT){    
            std::cout << "CONNECTING WITH " << msg.playerName << std::endl;                    
            if(clients.size() < 2) clients.push_back(std::move(client));

            if (clients.size() >= 2) 
            {
                std::cout << "Lobby full" << std::endl;

                Message startMsg;
                startMsg.type = Message::MessageType::START;
                startMsg.playerName = "Server";

                for(auto it = clients.begin(); it != clients.end(); ++it) socket.send(startMsg, **it);
            }
        }

        else if(msg.type == Message::MessageType::INPUT || msg.type == Message::MessageType::END){
            for(auto it = clients.begin(); it != clients.end(); ++it) if(!(**it == *client)) socket.send(msg, **it);

            std::cout << std::endl;
            std::cout << msg.type << std::endl;
            std::cout << msg.playerName << std::endl;
            std::cout << msg.nextTetraminoID << std::endl;
            std::cout << msg.input << std::endl;
            std::cout << std::endl;
        }

        else if(msg.type == Message::MessageType::DISCONNECT){
            std::cout << "DISCONNECTING " << msg.playerName << std::endl;
            auto it = clients.begin();
            while(it != clients.end() && !(**it == *client)) it++;
            if(it != clients.end()) clients.erase(it);
        }

        else std::cerr << "Error" << std::endl;
    }
}