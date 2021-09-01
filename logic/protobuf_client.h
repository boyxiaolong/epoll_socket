#ifndef logic_protobuf_client_h_
#define logic_protobuf_client_h_
#include "../include/client_sock.h"

#include <string>

namespace google {
    namespace protobuf {
        class Message;
    }
}

class protobuf_client : public client_sock {

public:

    protobuf_client(int ae_fd, int fd);

    virtual int read_data();

    virtual void process_data();

    virtual int handle_msg(int msg_id, const char* pdata, int length);

    int send_pb_msg(google::protobuf::Message* pmsg, int msg_id);
};
#endif