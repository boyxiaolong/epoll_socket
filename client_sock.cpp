#include "client_sock.h"
#include "log.h"

client_sock::client_sock(int ae_fd, int fd): ae_fd_(ae_fd)
, fd_(fd)
, max_length(16)
, cur_pos(0)
{
    buf_ = new char[max_length];
}
client_sock::~client_sock()
{
    if (NULL != buf_)
    {
        delete []buf_;
    }

    if (fd_ > 0)
    {
        close_sock();
    }
    
    LOG("fd %d dtor", fd_);
}
        
char* client_sock::get_data()
{
    return buf_+cur_pos;
}

int client_sock::get_left_length()
{
    return max_length-cur_pos;
}

void client_sock::process_data()
{
    //just f test
    if (get_left_length() > 0)
    {
        *(buf_+cur_pos+1) = '\0';
    }
    
    LOG("get data length %d data:%s", cur_pos, buf_);
    cur_pos = 0;
}

void client_sock::add_pos(int length)
{
    cur_pos += length;
    if (get_left_length() < 1)
    {
        int new_length = max_length*2;
        char* new_data = new char[new_length];
        memcpy(new_data, buf_, max_length);
        max_length = new_length;
        delete buf_;
        buf_ = new_data;
        LOG("resize data %d", new_length);
    }
}

int client_sock::read_data(){
    int readn = 0;
    bool is_read_error = false;
    while (true)
    {
        int nread = read(fd_, get_data(), get_left_length());
        if (nread < 0)
        {
            if (errno == EAGAIN)
            {
                LOG("fd %d read end!", fd_);
                break;
            }
            
            LOG("read error %d\n", nread);
            is_read_error = true;
            break;
        }
        if (nread == 0)
        {
            if (readn == 0)
            {
                LOG("read error");
                is_read_error = true;
            }
            else
            {
                LOG("readnum 0 so read end");
            }
            
            break;
        }
        LOG("scok %d read size %d left_size %d", fd_, nread, get_left_length());
        add_pos(nread);
        readn += nread;
        LOG("readnum:%d", nread);
    }
                
    if (is_read_error)
    {
        LOG("is_read_error");
        return -1;
    }

    process_data();
    return 0;
}

void client_sock::close_sock(){
    epoll_ctl(ae_fd_, EPOLL_CTL_DEL, fd_, NULL);
    close(fd_);
    fd_ = 0;
    ae_fd_ = 0;
}

int client_sock::get_fd(){
    return fd_;
}

int client_sock::set_noblock(){
    int flags = fcntl(fd_, F_GETFL);
    if (-1 == flags) {
        return -1;
    }

    return fcntl(fd_, F_SETFL, flags |O_NONBLOCK);
}

int client_sock::set_nodelay() {
    int val = 1;
    return setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(&val));
}

int client_sock::set_event(int event) {
    struct epoll_event ev;
    ev.events = event;
    ev.data.fd = fd_;
    if (epoll_ctl(ae_fd_, EPOLL_CTL_ADD, fd_, &ev) < 0)
    {
        LOG("error epoll_ctl");
        return -1;
    }
    return 0;
}