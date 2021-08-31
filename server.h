#ifndef _server_h__
#define _server_h__

#include <fcntl.h>
#include <errno.h>
#include <map>
#include <string>
#include "client_sock.h"

#define max_events 100
#define max_buff 1024
#define SERVER_PORT 9999
class Server
{
    public:
        Server();

        ~Server();

        void clear_data();

        int init_ae();

        virtual int create_server_sock(const char* ip, uint16_t port);

        virtual int ae_accept();

        client_sock* get_sock_ps(int cur_fd);

        virtual void rm_client_sock(int cur_fd);

        virtual int ae_poll();

        void set_running_flag(bool flag) { is_running_ = flag; }
        
    private:

        int ae_fd_;

        typedef std::map<int, client_sock*> socket_map;
        client_sock* pserver_sock_;

        int listen_fd_;

        int max_timeout_ms_;

        socket_map socket_map_;

        bool is_running_;
};

#endif