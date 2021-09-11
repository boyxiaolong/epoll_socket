#ifndef logic_protobuf_client_h_
#define logic_protobuf_client_h_
#include "../include/client_sock.h"
#include "../include/net_buffer.h"

#include <string>
#include <memory>

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

    void handle_logic();

    virtual void update();

private:
    typedef std::vector<std::shared_ptr<net_buffer> > net_buffer_vec;
    net_buffer_vec read_net_buffer_vec_;

    typedef std::vector<std::shared_ptr<google::protobuf::Message> > msg_vec;
    msg_vec msg_vec_;
};
#endif