#include "stdio.h"
#include "signal.h"
#include "pthread.h"
#include <unistd.h>

#include "include/server.h"
#include "include/log.h"
#include "logic/protobuf_client.h"
#include "logic/protobuf_server.h"

#include <memory>
#include <string>

#include "net_msg/login.pb.h"

std::atomic<int> is_running(true);

static void ctrl_handler(int sig){
    LOG("ctrl+c");
    is_running = false;
}

static void* sock_thread_handler(void* ser){
    if (NULL == ser) {
        LOG("thread error");
        return NULL;
    }
    
    server* pser = (server*)ser;
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

void naive_client_for_test() {
    sleep(3);

    std::unique_ptr<protobuf_client> pc(new protobuf_client(0, 0));
    int res = pc->sync_connect("0.0.0.0", 9999);
    if (res != 0) {
        LOG("connect error");
        return;
    }

    pc->set_noblock();
    pc->set_nodelay();

    example::Login login_msg;
    login_msg.set_msg_id(example::eMsgToSFromC_Login);
    login_msg.set_account_id("allen");
    login_msg.set_device_id(111);

    std::string msg_str = login_msg.SerializeAsString();

    int msg_size = msg_str.size();

    LOG("login_msg size %d",  msg_size);

    int msg_id = login_msg.msg_id();
    int total_size = 4 + 4 + msg_size;
    char* psend_data = new char[total_size];
    memcpy(psend_data, &total_size, sizeof(total_size));
    memcpy(psend_data + 4, &msg_id, sizeof(msg_id));
    memcpy(psend_data + 8, msg_str.c_str(), msg_size);
    
    pc->send_data(psend_data, total_size);

    //pc->send_data(psend_data, total_size);

    delete []psend_data;
}

int main() {
    
    signal(SIGINT, ctrl_handler);
    std::unique_ptr<server> pser(new protobuf_server);
    if (NULL == pser) {
        LOG("create server error");
        return -1;
    }

    pthread_t t;
    pthread_create(&t, NULL, sock_thread_handler, (void*)pser.get());

    naive_client_for_test();
    
    while (is_running) {
        sleep(1);
    }

    LOG("try to stop server");

    pser->set_running_flag(false);

    pthread_join(t, NULL);

    LOG("finish main");
    return 0;
}
