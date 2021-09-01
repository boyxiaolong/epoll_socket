#include "protobuf_server.h"

#include "protobuf_client.h"
#include "../include/log.h"

protobuf_server::protobuf_server() 
    : server() {
    LOG("create protobuf_server");
}

client_sock* protobuf_server::on_create_client(int ae_fd, int new_conn_fd) {
    client_sock* ps = new protobuf_client(ae_fd, new_conn_fd);
    LOG("on_create_client");
    return ps;
}


