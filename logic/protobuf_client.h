#ifndef logic_protobuf_client_h_
#define logic_protobuf_client_h_
#include "../include/client_sock.h"

#include <string>

class protobuf_client : public client_sock {

public:

    protobuf_client(int ae_fd, int fd);

    virtual int read_data();

    virtual void process_data();

    virtual int handle_msg(int msg_id, std::string& msg);

private:
    bool is_read_header_ = false;

    int header_size_ = 4;

    int msg_length_ = 0;

    int msg_id_ = -1;
};
#endif