#include "stdio.h"
#include "client_sock.h"
#include "server.h"
#include "signal.h"
#include "pthread.h"

bool is_running = true;
static void ctrl_handler(int sig){
    is_running = false;
}

static void* sock_thread_handler(void* ser){
    if (NULL == ser)
    {
        printf("thread error");
        return NULL;
    }
    
    Server* pser = (Server*)ser;
    int res = pser->init_ae();
    if (res < 0)
    {
        exit(-1);
        return NULL;
    }
    
    res = pser->create_server_sock();
    if (res < 0)
    {
        printf("create error\n");
        return NULL;
    }

    pser->ae_poll();
    return NULL;
}
int main()
{
    signal(SIGINT, ctrl_handler);
    Server* pser = new Server;
    pthread_t t;
    pthread_create(&t, NULL, sock_thread_handler, (void*)pser);
    while (is_running)
    {
        sleep(1);
    }
    pser->set_running_flag(false);
    pthread_join(t, NULL);
}
