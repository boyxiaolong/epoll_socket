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
        client_sock(int ae_fd, int fd);
        
        ~client_sock();
        
        char* get_data();

        int get_left_length();

        void process_data();

        void add_pos(int length);

        int read_data();

        void close_sock();

        int get_fd();

        int set_noblock();

        int set_nodelay();

        int set_event(int event);

    private:
        int ae_fd_;
        int fd_;
        int state_;
        char* buf_;
        int max_length;
        int cur_pos;
};
#endif