#include "protobuf_client.h"

protobuf_client::protobuf_client(int ae_fd, int fd) 
    : client_sock(ae_fd, fd) {

    }

int protobuf_client::read_data() {
    int res = client_sock::read_data();
    return res;
}


void protobuf_client::process_data() {
    client_sock::process_data();
}

