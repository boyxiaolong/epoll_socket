#include "stdio.h"
#include "client_sock.h"
#include "server.h"

int main()
{
    Server ser;
    int res = ser.init_ae();
    if (res < 0)
    {
        exit(-1);
    }
    
    res = ser.create_server_sock();
    if (res < 0)
    {
        printf("create error\n");
        return res;
    }

    ser.ae_poll();
}
