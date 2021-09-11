#include "protobuf_client.h"

#include "unistd.h"
#include <errno.h>

#include "../include/log.h"

#include "../net_msg/login.pb.h"
#include "../net_msg/login.pb.h"
#include "../net_msg/msg_num.pb.h"

#include <google/protobuf/message.h>

protobuf_client::protobuf_client(int ae_fd, int fd) 
    : client_sock(ae_fd, fd) {

    }

int protobuf_client::read_data() {
    LOG("begin read");
    while (true) {
        int nread = 0;

        int msg_length = 0;
        nread = read(fd_, &msg_length, 4);
        if (nread == 0) {
            LOG("read header error read_num %d error: %s", nread, strerror(errno));
            return -1;
        }

        if (nread < 0) {
            if (errno == EAGAIN) {
                break;
            }
        }

        if (nread != 4) {
            LOG("read header error read_num %d error: %s", nread, strerror(errno));
            return -1;
        }

        int msg_id = 0;
        nread = read(fd_, &msg_id, 4);
        if (nread != 4) {
            LOG("read header error");
            return -1;
        }

        int left_msg_len = msg_length - 8;
        std::shared_ptr<net_buffer> pbuff(new net_buffer(left_msg_len));
        pbuff->set_msg_id(msg_id);
        
        nread = read(fd_, pbuff->get_raw_data(), left_msg_len);
        if (nread != left_msg_len) {
            LOG("error read left_msg_len %d  real:%d errno: %d %s", left_msg_len, nread, errno, strerror(errno));
            return -1;
        }

        LOG("msg_length %d msg_id %d real_msg_len %d", msg_length, msg_id, left_msg_len);
        net_buffer_vec_.push_back(pbuff);
    }

    return 0;
}


void protobuf_client::process_data() {
}

int protobuf_client::handle_msg(std::shared_ptr<net_buffer> pnet_buffer) {
    switch (pnet_buffer->get_msg_id())
    {
    case game::eMsg_ReqLogin : {
            std::shared_ptr<game::ReqLogin> req_login_msg(new game::ReqLogin);
            req_login_msg->ParseFromArray(pnet_buffer->get_raw_data(), pnet_buffer->get_length());

            //msg_vec_.push_back(req_login_msg);
            //msg_vec_map[msg_id].push_back(req_login_msg);
            LOG("login account_id %s device_id %d", req_login_msg->account_id().c_str(), req_login_msg->device_id());

            game::ResLogin res_login_msg;
            res_login_msg.set_msg_id(game::eMsg_ResLogin);
            res_login_msg.set_actor_id(11);
            send_pb_msg(&res_login_msg, res_login_msg.msg_id());
        }
        break;
    
    default:
        LOG("not handle");
        break;
    }
    return 0;
}

int protobuf_client::send_pb_msg(google::protobuf::Message* pmsg, int msg_id) {
    std::string msg_str = pmsg->SerializeAsString();
    
    int msg_size = msg_str.size();

    int total_size = sizeof(int)*2 + msg_size;

    LOG("send msg size %d msg_id %d total_size %d",  msg_size, msg_id, total_size);

    char* psend_data = new char[total_size];
    memcpy(psend_data, (char*)&total_size, sizeof(int));
    memcpy(psend_data + sizeof(int), (char*)&msg_id, sizeof(int));
    memcpy(psend_data + sizeof(int)*2, (char*)msg_str.c_str(), msg_size);
    
    send_data(psend_data, total_size);

    delete []psend_data;

    return 0;
}

void protobuf_client::handle_logic() {
    for (auto pmsg : net_buffer_vec_) {
        handle_msg(pmsg);
    }
    net_buffer_vec_.clear();
}

void protobuf_client::update() {
    LOG("sock fd %d", fd_);
    handle_logic();
}