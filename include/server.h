#ifndef _server_h__
#define _server_h__

#include <map>

class client_sock;

class server {
    
    public:
        server();

        ~server();

        void clear_data();

        //初始化epoll
        int init_ae();

        //创建server
        virtual int create_server_sock(const char* ip, uint16_t port);

        //获取连接
        client_sock* get_sock_ps(int cur_fd);

        //删除连接
        virtual void rm_client_sock(int cur_fd);

        //epoll事件循环
        virtual int ae_poll();

        void set_running_flag(bool flag) { is_running_ = flag; }

        virtual client_sock* on_create_client(int ae_fd, int new_conn_fd);

    private:
        //处理新连接
        int _ae_accept();
        
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