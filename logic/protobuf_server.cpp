#include "protobuf_server.h"

#include "protobuf_client.h"

protobuf_server::protobuf_server() 
    : server() {

}

client_sock* protobuf_server::on_create_client(int ae_fd, int new_conn_fd) {
    client_sock* ps = new protobuf_client(ae_fd, new_conn_fd);
    return ps;
}


