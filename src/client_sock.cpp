#include "../include/client_sock.h"


#include "stdio.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "stdlib.h"
#include "netinet/in.h"
#include "string.h"
#include "unistd.h"
#include <fcntl.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include "netdb.h"

#include <string>

#include "../include/log.h"

client_sock::client_sock(int ae_fd, int fd): ae_fd_(ae_fd)
, fd_(fd) {
    if (fd_ > 0) {
        is_connected_ = true;
    }
    
    buf_ = new char[max_length_];
}

client_sock::~client_sock() {
    LOG("fd %d dtor", fd_);

    if (nullptr != buf_) {
        delete []buf_;
        buf_ = nullptr;
    }

    
    close_sock();
}
        
char* client_sock::get_data() {
    return buf_+ cur_pos_;
}

int client_sock::get_left_length() {
    return max_length_ - cur_pos_;
}

void client_sock::process_data() {
    //just f test
    cur_pos_ = 0;
}

void client_sock::add_pos(int length) {
    cur_pos_ += length;
}

int client_sock::read_data() {
    int readn = 0;
    
    bool is_read_error = false;

    while (true) {
        int nread = read(fd_, get_data(), get_left_length());
        if (nread < 0) {
            if (errno == EAGAIN)
            {
                LOG("fd %d read end!", fd_);
                break;
            }
            
            LOG("read error %d\n", nread);
            is_read_error = true;
            break;
        }
        if (nread == 0) {
            if (readn == 0) {
                LOG("read error");
                is_read_error = true;
            }
            else {
                LOG("readnum 0 so read end");
            }
            
            break;
        }
        LOG("scok %d read size %d left_size %d", fd_, nread, get_left_length());
        add_pos(nread);
        readn += nread;
        LOG("readnum:%d", nread);
    }
                
    if (is_read_error) {
        LOG("is_read_error");
        return -1;
    }

    process_data();
    return 0;
}

void client_sock::close_sock(){
    if (ae_fd_ > 0) {
        epoll_ctl(ae_fd_, EPOLL_CTL_DEL, fd_, NULL);
        ae_fd_ = 0;
    }
    
    if (fd_ > 0) {
        close(fd_);
        fd_ = 0;
    }
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
    if (epoll_ctl(ae_fd_, EPOLL_CTL_ADD, fd_, &ev) < 0) {
        LOG("error epoll_ctl");
        return -1;
    }
    return 0;
}

int client_sock::socket_init() {
    int res = set_noblock();
    if (res != 0) {
        LOG("set_noblock error");
        return -1;
    }

    res = set_nodelay();
    if (res != 0) {
        LOG("set_noblock error");
        return -1;
    }
    
    res = set_event(EPOLLIN);
    if (res != 0) {
        LOG("set_event error");
        return -1;
    }

    return 0;
}

int client_sock::sync_connect(const char* ip, uint16_t port) {
    LOG("begin connect ip %s port %d", ip, port);

    if (is_connected_) {
        LOG("already connected");
        return -1;
    }
    struct addrinfo serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.ai_family = AF_INET;
    serv_addr.ai_socktype = SOCK_STREAM;
    serv_addr.ai_protocol = IPPROTO_TCP;
    int conn_fd = socket(serv_addr.ai_family, serv_addr.ai_socktype, 0);
    if (conn_fd < 1) {
        LOG("socket create");
        return -1;    
    }

    struct addrinfo* real_server_info = NULL;
    char port_str[6];
    snprintf(port_str, 6, "%d", port);
    int res = getaddrinfo(ip, port_str, &serv_addr, &real_server_info);
    if (res != 0) {
        LOG("getaddrinfo error %d", res);
        return -1;
    }

    res = connect(conn_fd, (struct sockaddr *) (real_server_info->ai_addr), real_server_info->ai_addrlen);
    freeaddrinfo(real_server_info);
    if (res != 0) {
        LOG("connect error");
        return -1;
    }
    
    fd_ = conn_fd;
    
    LOG("end connect ip %s port %d success", ip, port);
    return 0;
}


int client_sock::send_data(char* pdata, int length) {
    int n = write(fd_, pdata, length);
    if (n != length) {
        return -1;
    }
    
    LOG("send length %d success", length);
    return 0;
}


void client_sock::expand_buf() {
    int new_length = max_length_*2;
    char* new_data = new char[new_length];
    memcpy(new_data, buf_, max_length_);
    max_length_ = new_length;
    delete []buf_;
    buf_ = new_data;
    LOG("resize data %d", new_length);
}