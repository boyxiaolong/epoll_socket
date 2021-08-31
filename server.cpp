#include "server.h"

Server::Server():is_running_(true),max_timeout_ms_(100),pserver_sock_(NULL) {

}

Server::~Server() {
    clear_data();
}

void Server::clear_data() {
    for (socket_map::iterator i = socket_map_.begin(); i != socket_map_.end(); ++i)
    {
        client_sock* ps = i->second;
        if (ps)
        {
            ps->close_sock();
            printf("close client %d\n", ps->get_fd());
            delete ps;
        }
    }
    socket_map_.clear();
    if (listen_fd_ > 0)
    {
        pserver_sock_->close_sock();
        listen_fd_ = 0;
        delete pserver_sock_;
    }

    printf("begin close ae_fd\n");
    if (ae_fd_ > 0)
    {
        close(ae_fd_);
        ae_fd_ = 0;
    }
    printf("server clear_data finish\n");
}

int Server::init_ae() {
    ae_fd_ = epoll_create(max_events);
    if (ae_fd_ < 0)
    {
        perror("epoll_create");
        return -1;
    }
    return 0;
}

int Server::create_server_sock() {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 1) 
    {
        perror("socket create");
        return -1;    
    }

    pserver_sock_ = new client_sock(ae_fd_, listen_fd_);

    pserver_sock_->set_noblock();
    pserver_sock_->set_nodelay();
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons (SERVER_PORT);
    serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    
    printf("begin bind on port %d\n", SERVER_PORT);
    int ret = bind(listen_fd_, (struct sockaddr *) (&serv_addr), sizeof(serv_addr));
    if (ret < 0)
    {
        perror("bind");
        return -1;
    }
    printf("bind success\n");
    
    ret = listen(listen_fd_, max_events);
    if (ret < 0)
    {
        perror("listen");
        return -1;
    }
    printf("listen success %d\n", listen_fd_);

    pserver_sock_->set_event(EPOLLIN);
}

int Server::ae_accept() {
    printf("begin ae_accept\n");
    int res = 0;
    do
    {
        struct sockaddr_in client_addr;
        int addrlen = 0;
        int new_socket = accept(listen_fd_, (struct sockaddr *)&client_addr,(socklen_t*)&addrlen);
        if (new_socket < 0)
        {
            if (errno == EAGAIN)
            {
                printf("fd %d acceptc nonblock!", ae_fd_);
                break;
            }
            res = -1;
            break;
        }
        client_sock* ps = new client_sock(ae_fd_, new_socket);
        ps->set_nodelay();
        ps->set_noblock();
        ps->set_event(EPOLLIN);
        printf("accept new socket %d and addto epoll\n", new_socket);
        
        socket_map_.insert(std::make_pair(new_socket, ps));
    } while (true);
    printf("end ae_accept\n");
    return res;
}

client_sock* Server::get_sock_ps(int cur_fd) {
    socket_map::iterator iter = socket_map_.find(cur_fd);
    if (iter == socket_map_.end())
    {
        return NULL;
    }

    return iter->second;
}

void Server::rm_client_sock(int fd) {
    socket_map::iterator iter = socket_map_.find(fd);
    if (iter == socket_map_.end())
    {
        return;
    }
    
    client_sock* ps = iter->second;
    socket_map_.erase(iter);

    if (NULL == ps)
    {
        return;
    }
    
    ps->close_sock();
    delete ps;    
    printf("delete fd:%d client close socket\n", fd);
}

int Server::ae_poll() {
    struct epoll_event events[max_events];
    while (is_running_)
    {
        //printf("begin epoll\n");
        int nfds = epoll_wait(ae_fd_, events, max_events, max_timeout_ms_);
        if (nfds == 0)
        {
            continue;
        }
        
        if (nfds == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            
            perror("wait");
            exit(-1);
        }
        printf("get epoll data %d\n", nfds);
        for (size_t i = 0; i < nfds; i++)
        {
            epoll_event& cur_ev = events[i];
            int cur_fd = cur_ev.data.fd;
            printf("epoll cur_fd %d\n", cur_fd);
            if (cur_fd == listen_fd_)
            {
                ae_accept();
            }
            else
            {
                client_sock* ps = get_sock_ps(cur_fd);
                if (NULL == ps)
                {
                    continue;
                }
                int res = ps->read_data();
                if (res < 0)
                {
                    rm_client_sock(cur_fd);
                }
            }
        }
    }
    printf("server finish\n");
    return 0;
}
