#include "protobuf_client.h"

#include "unistd.h"
#include <errno.h>

#include "../include/log.h"

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
                LOG("read header error");
                is_read_error = true;
                break;
            }

            LOG("msg_length %d", msg_length_);
            is_read_header_ = true;
        }
        else {
            int left_msg_len = msg_length_ - cur_pos;
            LOG("left_msg_len %d", left_msg_len);
            nread = read(fd_, get_data(), left_msg_len);
        }
        
        if (nread < 0) {
            if (errno == EAGAIN)
            {
                LOG("fd %d read end!", fd_);
                break;
            }
            
            LOG("read error %d\n", nread);
            is_read_error = true;
            break;
        }
        if (nread == 0) {
            if (cur_pos < msg_length_) {
                LOG("read error");
                is_read_error = true;
            }
            else {
                LOG("readnum 0 so read end");
            }
            
            break;
        }
        LOG("scok %d read size %d left_size %d", fd_, nread, get_left_length());
        add_pos(nread);
        readn += nread;
        LOG("readnum:%d", nread);
    }
                
    if (is_read_error) {
        LOG("is_read_error");
        return -1;
    }

    process_data();
    return 0;
}


void protobuf_client::process_data() {
    client_sock::process_data();
}

