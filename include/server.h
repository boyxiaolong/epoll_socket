#ifndef _server_h__
#define _server_h__

#include <map>
#include <memory>

#include "net_timer.h"

class client_sock;

class server {
    public:
        server();

        virtual ~server();

        //清楚数据
        void clear_data();

        //初始化epoll
        int init_ae();

        //创建server
        virtual int create_server_sock(const char* ip, uint16_t port);

        //获取连接
        std::shared_ptr<client_sock> get_sock_ps(int cur_fd);

        //epoll事件循环
        virtual int ae_poll();

        void set_running_flag(bool flag) { is_running_ = flag; }

        //创建client
        virtual client_sock* on_create_client(int ae_fd, int new_conn_fd);

        int get_ae_fd(){return ae_fd_;}

        void update();

    private:
        //处理新连接
        int _ae_accept();
        
    private:
        //epoll fd
        int ae_fd_ = 0;

        //监听socket实例
        std::unique_ptr<client_sock> pserver_sock_;

        typedef std::map<int, std::shared_ptr<client_sock> > socket_map;

        int listen_fd_ = 0;

        //最大延迟时间
        int max_timeout_ms_ = 100;

        //连接map
        socket_map socket_map_;

        //是否在运行
        bool is_running_ = true;
};

#endif