#ifndef logic_protobuf_net_buffer_h_
#define logic_protobuf_net_buffer_h_
#include <vector>

class net_buffer
{
public:
    net_buffer(int total_size);

    char* get_raw_data();
    int get_length(){return data_.size();}

    void resize(int max_size);

    void set_msg_id(int msg_id) { msg_id_ = msg_id; }

    int get_msg_id() { return msg_id_; }

private:
    std::vector<char> data_;

    int msg_id_ = 0;
};

#endif
