#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):pos_x(_x),pos_y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        alloc_data(sizeof(int16_t) * 2 + sizeof(char) * MAX_NAME);

        char* aux = _data;

        memcpy(aux, &pos_x, sizeof(int16_t));
        aux += sizeof(int16_t);
        memcpy(aux, &pos_y, sizeof(int16_t));
        aux += sizeof(int16_t);
        memcpy(aux, name, MAX_NAME);
    }

    int from_bin(char* data)
    {
        return 0;
    }


private:
    int16_t pos_x;
    int16_t pos_y;

    static const size_t MAX_NAME = 20;

    char name[MAX_NAME];
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int fd = open("./playerdata", O_CREAT| O_WRONLY | O_TRUNC, 0666);
    write(fd, one_w.data(), one_w.size());
    close(fd);

    // 3. Leer el fichero
    // 4. "Deserializar" en one_r
    // 5. Mostrar el contenido de one_r

    return 0;
}