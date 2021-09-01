#include "stdio.h"
#include "signal.h"
#include "pthread.h"
#include <unistd.h>

#include "include/server.h"
#include "include/log.h"

#include <memory>

#include "net_msg/login.pb.h"

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
    protobuf_login_2eproto::example::Login login_msg;
    signal(SIGINT, ctrl_handler);
    std::unique_ptr<Server> pser(new Server);
    if (NULL == pser) {
        LOG("create server error");
        return -1;
    }

    pthread_t t;
    pthread_create(&t, NULL, sock_thread_handler, (void*)pser.get());

    while (is_running) {
        sleep(1);
    }

    LOG("try to stop server");

    pser->set_running_flag(false);

    pthread_join(t, NULL);

    pser->clear_data();

    LOG("finish main");
    return 0;
}
