#include "../include/server.h"

#include "stdio.h"
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "stdlib.h"
#include "netinet/in.h"
#include "string.h"
#include "unistd.h"
#include "netdb.h"

#include "../include/log.h"

Server::Server():is_running_(true),max_timeout_ms_(100),pserver_sock_(NULL) {

}

Server::~Server() {
    clear_data();
}

void Server::clear_data() {
    for (socket_map::iterator i = socket_map_.begin(); i != socket_map_.end(); ++i) {
        client_sock* ps = i->second;
        if (ps) {
            ps->close_sock();
            LOG("close client %d", ps->get_fd());
            delete ps;
        }
    }
    socket_map_.clear();
    if (listen_fd_ > 0) {
        pserver_sock_->close_sock();
        listen_fd_ = 0;
        delete pserver_sock_;
    }

    LOG("begin close ae_fd");
    if (ae_fd_ > 0) {
        close(ae_fd_);
        ae_fd_ = 0;
    }
    LOG("server clear_data finish");
}

int Server::init_ae() {
    ae_fd_ = epoll_create(max_events);
    if (ae_fd_ < 0) {
        LOG("error epoll_create");
        return -1;
    }
    return 0;
}

int Server::create_server_sock(const char* ip, uint16_t port) {
    struct addrinfo serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.ai_family = AF_INET;
    serv_addr.ai_socktype = SOCK_STREAM;
    serv_addr.ai_protocol = IPPROTO_TCP;
    listen_fd_ = socket(serv_addr.ai_family, serv_addr.ai_socktype, 0);
    if (listen_fd_ < 1) {
        LOG("socket create");
        return -1;    
    }

    struct addrinfo* real_server_info = NULL;
    char port_str[6];
    snprintf(port_str, 6, "%d", port);
    int res = getaddrinfo(ip, port_str, &serv_addr, &real_server_info);
    if (res < 0) {
        LOG("getaddrinfo error %d", res);
        close(listen_fd_);
        return -1;
    }

    LOG("begin bind on port %d", port);
    int ret = bind(listen_fd_, (struct sockaddr *) (real_server_info->ai_addr), real_server_info->ai_addrlen);
    freeaddrinfo(real_server_info);
    if (ret < 0) {
        LOG("bind");
        close(listen_fd_);
        return -1;
    }
    LOG("bind success");
    
    ret = listen(listen_fd_, max_events);
    if (ret < 0) {
        LOG("listen error");
        close(listen_fd_);
        return -1;
    }

    LOG("listen success %d", listen_fd_);

    pserver_sock_ = new client_sock(ae_fd_, listen_fd_);
    pserver_sock_->set_noblock();
    pserver_sock_->set_nodelay();
    pserver_sock_->set_event(EPOLLIN);
    return 0;
}

int Server::ae_accept() {
    LOG("begin ae_accept");
    int res = 0;
    do {
        struct sockaddr_in client_addr;
        int addrlen = 0;
        int new_socket = accept(listen_fd_, (struct sockaddr *)&client_addr,(socklen_t*)&addrlen);
        if (new_socket < 0) {
            if (errno == EAGAIN) {
                LOG("fd %d acceptc nonblock!", ae_fd_);
                break;
            }
            res = -1;
            break;
        }
        client_sock* ps = new client_sock(ae_fd_, new_socket);
        ps->set_nodelay();
        ps->set_noblock();
        ps->set_event(EPOLLIN);
        LOG("accept new socket %d and addto epoll", new_socket);
        
        socket_map_.insert(std::make_pair(new_socket, ps));
    } while (true);

    LOG("end ae_accept");
    return res;
}

client_sock* Server::get_sock_ps(int cur_fd) {
    socket_map::iterator iter = socket_map_.find(cur_fd);
    if (iter == socket_map_.end()) {
        return NULL;
    }

    return iter->second;
}

void Server::rm_client_sock(int fd) {
    socket_map::iterator iter = socket_map_.find(fd);
    if (iter == socket_map_.end()) {
        return;
    }
    
    client_sock* ps = iter->second;
    socket_map_.erase(iter);

    if (NULL == ps) {
        return;
    }
    
    ps->close_sock();
    delete ps;    
    LOG("delete fd:%d client close socket", fd);
}

int Server::ae_poll() {
    struct epoll_event events[max_events];
    while (is_running_) {
        //printf("begin epoll\n");
        int nfds = epoll_wait(ae_fd_, events, max_events, max_timeout_ms_);
        if (nfds == 0) {
            continue;
        }
        
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            }
            
            perror("wait");
            exit(-1);
        }

        LOG("get epoll data %d", nfds);

        for (size_t i = 0; i < nfds; i++) {
            epoll_event& cur_ev = events[i];
            int cur_fd = cur_ev.data.fd;
            LOG("epoll cur_fd %d", cur_fd);
            if (cur_fd == listen_fd_) {
                ae_accept();
            }
            else {
                client_sock* ps = get_sock_ps(cur_fd);
                if (NULL == ps) {
                    continue;
                }
                int res = ps->read_data();
                if (res < 0) {
                    rm_client_sock(cur_fd);
                }
            }
        }
    }
    LOG("server finish");
    return 0;
}
