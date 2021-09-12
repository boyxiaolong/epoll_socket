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
#include <fcntl.h>

#include "../include/log.h"
#include "../include/client_sock.h"
#include "../include/net_timer.h"

#include <string>

#define max_events 100
#define max_buff 1024

server::server() {
}

server::~server() {
    clear_data();
}

void server::clear_data() {
    for (socket_map::iterator i = socket_map_.begin(); i != socket_map_.end(); ++i) {
        std::shared_ptr<client_sock> ps = i->second;

        if (NULL == ps) {
            continue;
        }
        
        LOG("close client %d", ps->get_fd());

        ps->close_sock();
    }
    socket_map_.clear();

    if (pserver_sock_) {
        pserver_sock_->close_sock();
        listen_fd_ = 0;
    }

    LOG("begin close ae_fd");

    if (ae_fd_ > 0) {
        close(ae_fd_);
        ae_fd_ = 0;
    }

    LOG("server clear_data finish");
}

int server::init_ae() {
    ae_fd_ = epoll_create(max_events);

    if (ae_fd_ < 0) {
        LOG("error epoll_create");
        return -1;
    }

    return 0;
}

int server::create_server_sock(const char* ip, uint16_t port) {
    addrinfo serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.ai_family = AF_INET;
    serv_addr.ai_socktype = SOCK_STREAM;
    serv_addr.ai_protocol = IPPROTO_TCP;
    listen_fd_ = socket(serv_addr.ai_family, serv_addr.ai_socktype, 0);
    if (listen_fd_ < 1) {
        LOG("socket create");
        return -1;    
    }

    addrinfo* real_server_info = NULL;
    char port_str[6];
    snprintf(port_str, 6, "%d", port);
    int res = getaddrinfo(ip, port_str, &serv_addr, &real_server_info);
    if (res < 0) {
        LOG("getaddrinfo error %d", res);
        close(listen_fd_);
        return -1;
    }

    LOG("begin bind on port %d", port);
    int ret = bind(listen_fd_, static_cast<sockaddr *>(real_server_info->ai_addr), real_server_info->ai_addrlen);
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

    pserver_sock_ = std::make_unique<client_sock>(ae_fd_, listen_fd_);
    pserver_sock_->set_noblock();
    pserver_sock_->set_nodelay();
    pserver_sock_->set_event(true, false);
    
    return 0;
}

int server::_ae_accept() {
    LOG("begin ae_accept");
    int res = 0;
    do {
        sockaddr_in client_addr;
        int addrlen = 0;
        int new_socket = accept(listen_fd_,  (sockaddr *)(&client_addr), (socklen_t*)(&addrlen));
        if (new_socket < 0) {
            if (errno == EAGAIN) {
                LOG("fd %d acceptc nonblock!", ae_fd_);
                break;
            }
            res = -1;
            break;
        }

        
        client_sock* pclient = on_create_client(ae_fd_, new_socket);
        if (nullptr == pclient) {
            LOG("create client error");
            close(new_socket);
            continue;
        }

        std::shared_ptr<client_sock> pnew_client;
        pnew_client.reset(pclient);

        int res = pnew_client->socket_init();
        if (res != 0) {
            LOG("socket_init error");
        }

        LOG("add new socket %d", new_socket);
        socket_map_.insert(std::make_pair(new_socket, pnew_client));
    } while (true);

    LOG("end ae_accept");
    return res;
}

std::shared_ptr<client_sock> server::get_sock_ps(int cur_fd) {
    socket_map::iterator iter = socket_map_.find(cur_fd);
    if (iter == socket_map_.end()) {
        return NULL;
    }

    return iter->second;
}

int server::ae_poll() {
    LOG("begin poll");
    
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

        for (size_t i = 0; i < nfds; i++) {
            epoll_event& cur_ev = events[i];
            int cur_fd = cur_ev.data.fd;
            int event = cur_ev.events;

            if (cur_fd == listen_fd_) {
                _ae_accept();
            } 
            else {
                std::shared_ptr<client_sock> ps = get_sock_ps(cur_fd);
                if (NULL == ps) {
                    continue;
                }

                if (event & EPOLLIN) {
                    int res = ps->on_read();
                    if (res < 0) {
                        ps->on_disconnect();
                    }
                }
                else if (event & EPOLLOUT){
                    int res = ps->on_write();
                    if (res < 0) {
                        ps->on_disconnect();
                    }
                }
            }
        }

        update();
    }
    LOG("server finish");
    return 0;
}


client_sock* server::on_create_client(int ae_fd, int new_conn_fd) {
    client_sock* ps = new client_sock(ae_fd_, new_conn_fd);
    return ps;
}

void server::update() {
    for (socket_map::iterator iter = socket_map_.begin();
    iter != socket_map_.end(); ) {
        std::shared_ptr<client_sock> psock = iter->second;
        if (!psock) {
            continue;
        }

        psock->update();

        if (psock->get_state() == socket_close) {
            psock->close_sock();

            socket_map_.erase(iter++);
            continue;
        }
        
        ++iter;
    }

    net_timer::get_instance()->update();
}