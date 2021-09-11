#include "net_buffer.h"

net_buffer::net_buffer(int total_size) : data_(total_size, 0) {

}

char* net_buffer::get_raw_data() {
    return &data_[0];
}

void net_buffer::resize(int max_size) {
    if (data_.capacity() < max_size) {
        data_.resize(max_size);
    }
}

