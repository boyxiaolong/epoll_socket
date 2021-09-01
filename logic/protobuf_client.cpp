#include "protobuf_client.h"

#include "unistd.h"
#include <errno.h>

#include "../include/log.h"

#include "../net_msg/req_login.pb.h"
#include "../net_msg/msg_num.pb.h"

#include <google/protobuf/message.h>

protobuf_client::protobuf_client(int ae_fd, int fd) 
    : client_sock(ae_fd, fd) {

    }

int protobuf_client::read_data() {
    LOG("begin read");
    
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
    
    nread = read(fd_, buf_, left_msg_len);
    if (nread != left_msg_len) {
        LOG("read header error");
        return -1;
    }

    LOG("msg_length %d msg_id %d real_msg_len %d", msg_length, msg_id, left_msg_len);
    handle_msg(msg_id, buf_, left_msg_len);

    return 0;
}


void protobuf_client::process_data() {
}

int protobuf_client::handle_msg(int msg_id, const char* pdata, int length) {
    switch (msg_id)
    {
    case game::eMsg_ReqLogin : {
            game::ReqLogin login_msg;
            login_msg.ParseFromArray(pdata, length);
            LOG("login account_id %s device_id %d", login_msg.account_id().c_str(), login_msg.device_id());
        }
        break;
    
    default:
        LOG("msgid %d not handle", msg_id);
        break;
    }
    return 0;
}

int protobuf_client::send_pb_msg(google::protobuf::Message* pmsg, int msg_id) {
    std::string msg_str = pmsg->SerializeAsString();
    
    int msg_size = msg_str.size();

    LOG("send msg size %d",  msg_size);

    int total_size = 4 + 4 + msg_size;
    char* psend_data = new char[total_size];
    memcpy(psend_data, &total_size, sizeof(total_size));
    memcpy(psend_data + 4, &msg_id, sizeof(msg_id));
    memcpy(psend_data + 8, msg_str.c_str(), msg_size);
    
    send_data(psend_data, total_size);

    delete []psend_data;

    return 0;
}