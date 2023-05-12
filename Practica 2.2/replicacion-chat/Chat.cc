#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* aux = _data;
    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    memcpy(aux, nick.c_str(), sizeof(char) * 8);
    aux += sizeof(char) * 8;
    memcpy(aux, message.c_str(), sizeof(char) * 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* aux = _data;

    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    nick = aux;
    aux += sizeof(char) * 8;
    message = aux;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
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
        
        ChatMessage msg;
	    Socket *msgSocket;
        
        int res = socket.recv(msg, msgSocket);

        if(res < 0) continue;

        std::unique_ptr<Socket> client(msgSocket);

        std::cout << "NICK: " << msg.nick << "TYPE: " << unsigned(msg.type) << "MESSAGGE: " << msg.type << std::endl;

        if (msg.type == ChatMessage::MessageType::LOGIN){    
            std::cout << "LOGIN " << msg.nick << "\n";                    
            clients.push_back(std::move(client));
        }

        else if(msg.type == ChatMessage::MessageType::MESSAGE){
            std::cout << "MESSAGGE " << msg.message << " From: " << msg.nick << "\n";
            for(auto it = clients.begin(); it != clients.end(); ++it) if(!(**it == *client)) socket.send(msg, **it);
        }

        else if(msg.type == ChatMessage::MessageType::LOGOUT){
            std::cout << "LOGOUT " << msg.nick << "\n";
            auto it = clients.begin();
            while(it != clients.end() && !(**it == *client)) it++;
            if(it != clients.end()) clients.erase(it);
        }

        else std::cerr << "Error" << std::endl;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::cout << "LOGIN";
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
    std::string msg;

    ChatMessage om(nick, msg);
    om.type = ChatMessage::LOGOUT;

    socket.send(om, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string str;
        std::getline(std::cin, str);
        // Enviar al servidor usando socket
        ChatMessage msg = ChatMessage(nick, str);
        msg.type = ChatMessage::MessageType::MESSAGE;
        socket.send(msg, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        ChatMessage msg;

        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        if(socket.recv(msg) >= 0) std::cout << msg.nick << ": " << msg.message << std::endl;
    }
}

