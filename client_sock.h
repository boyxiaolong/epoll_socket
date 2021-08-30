#ifndef _client_sock_h__
#define _client_sock_h__
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

class client_sock
{
    public:
        client_sock(int fd):fd_(fd)
        , max_length(16)
        , cur_pos(0)
        {
            buf_ = new char[max_length];
        }
        ~client_sock()
        {
            if (NULL != buf_)
            {
                delete []buf_;
            }

            if (fd_ > 0)
            {
                close_sock();
            }
            
            printf("fd %d dtor\n", fd_);
        }
        
        char* get_data()
        {
            return buf_+cur_pos;
        }

        int get_left_length()
        {
            return max_length-cur_pos;
        }

        void process_data()
        {
            //just f test
            if (get_left_length() > 0)
            {
                *(buf_+cur_pos+1) = '\0';
            }
            
            printf("get data length %d data:%s\n", cur_pos, buf_);
            cur_pos = 0;
        }

        void add_pos(int length)
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
                printf("resize data %d\n", new_length);
            }
        }

        int read_data(){
            int readn = 0;
            bool is_read_error = false;
            while (true)
            {
                int nread = read(fd_, get_data(), get_left_length());
                if (nread < 0)
                {
                    if (errno == EAGAIN)
                    {
                        printf("fd %d read end!", fd_);
                        break;
                    }
                    
                    printf("read error %d\n", nread);
                    is_read_error = true;
                    break;
                }
                if (nread == 0)
                {
                    if (readn == 0)
                    {
                        printf("read error 2\n");
                        is_read_error = true;
                    }
                    else
                    {
                        printf("readnum 0 so read end\n");
                    }
                    
                    break;
                }
                printf("scok %d read size %d\n left_size %d\n", fd_, nread, get_left_length());
                add_pos(nread);
                readn += nread;
                printf("readnum:%d\n", nread);
            }
                        
            if (is_read_error)
            {
                printf("is_read_error\n");
                return -1;
            }

            process_data();
            return 0;
        }

        void close_sock(){
            close(fd_);
        }

        int get_fd(){
            return fd_;
        }

        int set_noblock(){
            int flags = fcntl(fd_, F_GETFL);
            if (-1 == flags) {
                return -1;
            }

            return fcntl(fd_, F_SETFL, flags |O_NONBLOCK);
        }

        int set_nodelay() {
            int val = 1;
            return setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(&val));
        }

        int set_event(int ep_fd, int event) {
            struct epoll_event ev;
            ev.events = event;
            ev.data.fd = fd_;
            if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd_, &ev) < 0)
            {
                perror("epoll_ctl");
                return -1;
            }
            return 0;
        }

    private:
        int fd_;
        int state_;
        char* buf_;
        int max_length;
        int cur_pos;
};
#endif