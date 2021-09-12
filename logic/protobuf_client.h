#ifndef logic_protobuf_client_h_
#define logic_protobuf_client_h_
#include "../include/client_sock.h"
#include "../include/net_buffer.h"

#include <string>
#include <memory>
#include <queue>

namespace google {
    namespace protobuf {
        class Message;
    }
}

class net_buffer;

class protobuf_client : public client_sock {
public:
    protobuf_client(int ae_fd, int fd);

    virtual int read_data();

    virtual void process_data();

    virtual int handle_msg(std::shared_ptr<net_buffer> pnet_buffer);

    int send_pb_msg(google::protobuf::Message* pmsg, int msg_id);

private:
    typedef std::queue<std::shared_ptr<google::protobuf::Message> > msg_vec;
    msg_vec msg_vec_;
};
#endif