#include "stdio.h"
#include "signal.h"
#include "pthread.h"
#include <unistd.h>

#include "include/server.h"
#include "include/log.h"

bool is_running = true;
static void ctrl_handler(int sig){
    LOG("ctrl+c");
    is_running = false;
}

static void* sock_thread_handler(void* ser){
    if (NULL == ser) {
        LOG("thread error");
        return NULL;
    }
    
    Server* pser = (Server*)ser;
    int res = pser->init_ae();
    if (res < 0) {
        LOG("init_ae error");
        return NULL;
    }
    
    res = pser->create_server_sock("0.0.0.0", 9999);
    if (res < 0) {
        LOG("create error");
        return NULL;
    }

    pser->ae_poll();
    return NULL;
}

int main() {
    signal(SIGINT, ctrl_handler);
    Server* pser = new Server;
    pthread_t t;
    pthread_create(&t, NULL, sock_thread_handler, (void*)pser);
    while (is_running) {
        sleep(1);
    }
    LOG("try to stop server");
    pser->set_running_flag(false);
    pthread_join(t, NULL);
    pser->clear_data();
    delete pser;
    LOG("finish main");
}
