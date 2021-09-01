#ifndef logic_protobuf_server_h_
#define logic_protobuf_server_h_

#include "../include/server.h"

class client_sock;

class protobuf_server : public server {

public:

    protobuf_server();

    virtual client_sock* on_create_client(int ae_fd, int new_conn_fd);
};
#endif