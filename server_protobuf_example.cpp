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
#include <vector>

#include "net_msg/login.pb.h"
#include "net_msg/msg_num.pb.h"

std::atomic<int> is_running(true);

static void ctrl_handler(int sig) {
    LOG("ctrl+c");
    is_running = false;
}

static void* sock_thread_handler(void* ser) {
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

static void* client_handler(void* ser) {
    std::unique_ptr<protobuf_client> pc(new protobuf_client(0, 0));
    int res = pc->sync_connect("0.0.0.0", 9999);
    if (res != 0) {
        LOG("connect error");
        return NULL;
    }

    //pc->set_noblock();
    pc->set_nodelay();

    game::ReqLogin login_msg;
    login_msg.set_msg_id(game::eMsg_ReqLogin);
    login_msg.set_account_id("allen");
    login_msg.set_device_id(111);

    int msg_id = login_msg.msg_id();
    
    pc->send_pb_msg(&login_msg, msg_id);
    pc->send_pb_msg(&login_msg, msg_id);
    sleep(10);
    //pc->read_data();
    LOG("thread finish!");
    return NULL;
}

void naive_client_for_test(int thread_num) {
    std::vector< pthread_t*> pvec;
    for (size_t i = 0; i < thread_num; i++){
        pthread_t* pt = new pthread_t;
        pthread_create(pt, NULL, client_handler, NULL);
        pvec.push_back(pt);
    }

    for (size_t i = 0; i < thread_num; i++) {
        pthread_t* pt = pvec[i];
        pthread_join(*pt, NULL);
        delete pt;
    }
}

enum program_type_enum {
    program_type_server = 0,
    program_type_client = 1
};

int main(int argc, char* argv[]) {
    signal(SIGINT, ctrl_handler);

    int program_type = 0;

    if (argc > 1) {
        program_type = atoi(argv[1]);
        LOG("program_type %d", program_type); 
    }
    
    if (program_type_client == program_type) {
        int thread_num = 0;
        if (argc > 2) {
            thread_num = atoi(argv[2]);
        }
        
        naive_client_for_test(thread_num);
        return 0;
    }
    
    std::unique_ptr<server> pser(new protobuf_server);
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

    LOG("finish main");
    return 0;
}
