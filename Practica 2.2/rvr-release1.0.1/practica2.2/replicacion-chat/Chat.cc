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
    memcpy(aux, &nick.c_str, sizeof(char) * 8);
    aux += sizeof(char) * 8;
    memcpy(aux, message.c_str, sizeof(char) * 80);
    aux += sizeof(char) * 80;
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* aux = _data;

    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);
    memcpy(&nick.c_str, aux, sizeof(char) * 8);
    aux += sizeof(char) * 8;
    memcpy(name, aux, sizeof(char) * 80);
    aux += sizeof(char) * 80;

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
	    Socket *msgSocket = new Socket(socket);
	    socket.recv(msg, msgSocket);

        switch(msg.type) {
            case LOGIN:
                std::unique_ptr<Socket> uptr = std::make_unique<Socket>(*msgSocket);
		        msgSock = nullptr;
		        clients.push_back(std::move(uptr));
                break;

            case LOGOUT:
                auto it = clients.begin();
	            while(it != clients.end() && **it != *msgSocket) it++;
	            if(it == clients.end()) return;
                clients.erase(it);
                break;

            case MESSAGE:
                for(auto it = clients.begin(); it! = clients.end(); ++it) if(**it != *msgSocket) socket.send(msg, **it);
                break;
        }

    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
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
        msg.type = MESSAGE;
        socket.send(msg, socket);
    }
}

void ChatClient::net_thread()
{
    Socket* recSocket = new Socket(socket);

    while(true)
    {
        //Recibir Mensajes de red
        ChatMessage msg;
        socket.recv(msg, recSocket);

        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << msg.nick << ": " << msg.message << std::endl;
    }
}


