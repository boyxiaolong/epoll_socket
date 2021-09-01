#include "protobuf_client.h"

#include "unistd.h"
#include <errno.h>

#include "../include/log.h"

#include "../net_msg/login.pb.h"

protobuf_client::protobuf_client(int ae_fd, int fd) 
    : client_sock(ae_fd, fd) {

    }

int protobuf_client::read_data() {
    LOG("begin read");
    int readn = 0;
    
    int nread = 0;
    int msg_length = 0;
    nread = read(fd_, &msg_length, 4);
    if (nread != 4) {
        LOG("read header error read_num %d", nread);
        return -1;
    }
    int msg_id = 0;
    nread = read(fd_, &msg_id, 4);
    if (nread != 4) {
        LOG("read header error");
        return -1;
    }

    int left_msg_len = msg_length - 8;
    //LOG("left_msg_len %d", left_msg_len);
    while (left_msg_len > max_length_) {
       expand_buf();
    }
    
    nread = read(fd_, buf_, max_length_);
    if (nread != left_msg_len) {
        LOG("read header error");
        return -1;
    }

    LOG("msg_length %d msg_id %d real_msg_len %d", msg_length, msg_id, left_msg_len);
    std::string msg(buf_, left_msg_len);
    handle_msg(msg_id, msg);

    return 0;
}


void protobuf_client::process_data() {
}

int protobuf_client::handle_msg(int msg_id, std::string& msg) {
    switch (msg_id)
    {
    case example::eMsgToSFromC_Login: {
            example::Login login_msg;
            login_msg.ParseFromString(msg);
            LOG("login account_id %s device_id %d", login_msg.account_id().c_str(), login_msg.device_id());
        }
        break;
    
    default:
        LOG("msgid %d not handle", msg_id);
        break;
    }
    return 0;
}