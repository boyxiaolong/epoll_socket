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
    bool is_read_error = false;
    while (true) {
        int nread = 0;
        if (!is_read_header_) {
            nread = read(fd_, &msg_length_, 4);
            if (nread != 4) {
                LOG("read header error read_num %d", nread);
                is_read_error = true;
                break;
            }
            is_read_header_ = true;
        }
        else if (msg_id_ < 0) {
            nread = read(fd_, &msg_id_, 4);
            if (nread != 4) {
                LOG("read header error");
                is_read_error = true;
                break;
            }
        }
        else {
            int left_msg_len = msg_length_ - 8;
            //LOG("left_msg_len %d", left_msg_len);
            nread = read(fd_, get_data(), left_msg_len);
            if (nread != left_msg_len) {
                LOG("read header error");
                is_read_error = true;
                break;
            }

            add_pos(nread);
            break;
        }
    }
                
    if (is_read_error) {
        LOG("is_read_error");
        return -1;
    }

    process_data();
    return 0;
}


void protobuf_client::process_data() {
    std::string msg(buf_, msg_length_-8);
    handle_msg(msg_id_, msg);
}

int protobuf_client::handle_msg(int msg_id, std::string& msg) {
    switch (msg_id)
    {
    case example::eMsgToSFromC_Login: {
            example::Login login_msg;
            login_msg.ParseFromString(msg);
            LOG("login msg %s size %d account_id %s device_id %d"
            , msg.c_str(), msg.size(), login_msg.account_id().c_str(), login_msg.device_id());
        }
        break;
    
    default:
        LOG("msgid %d not handle", msg_id);
        break;
    }
    return 0;
}