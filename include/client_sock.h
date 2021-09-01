#ifndef _client_sock_h__
#define _client_sock_h__

#include <stdint.h>

class client_sock {
    
    public:
        client_sock(int ae_fd, int fd);
        
        virtual ~client_sock();
        
        //处理读取好的数据
        virtual void process_data();

        //socket初始化工作
        virtual int socket_init();

        //获取buff指针
        char* get_data();

        //buff剩余长度
        int get_left_length();

        //从网络读数据
        virtual int read_data();

        //关闭套接字资源
        void close_sock();

        //获取套接字
        int get_fd();

        //设置非阻塞
        int set_noblock();

        //设置无延迟
        int set_nodelay();

        //设置读写事件
        int set_event(int event);

        //同步连接
        int sync_connect(const char* ip, uint16_t port);

        int send_data(char* pdata, int length);

    protected:
        void add_pos(int length);

        void expand_buf();

    protected:
        bool is_connected_ = false;
        int ae_fd_;
        int fd_;
        int state_;
        char* buf_;
        int max_length_;
        int cur_pos;
};
#endif