#ifndef logic_protobuf_client_h_
#define logic_protobuf_client_h_
#include "../include/client_sock.h"

class protobuf_client : client_sock {

public:

    protobuf_client(int ae_fd, int fd);

    virtual int read_data();

    virtual void process_data();
};
#endif