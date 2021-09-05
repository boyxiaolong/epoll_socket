#ifndef _client_sock_h__
#define _client_sock_h__

#include <stdint.h>

#include <vector>

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

        //发送数据
        int send_data(char* pdata, int length);

        int set_timeout();

    protected:
        void add_pos(int length);

        //buf扩展
        void expand_buf();

    protected:
        //是否已连接
        bool is_connected_ = false;

        //epoll fd
        int ae_fd_ = 0;

        //本socket fd
        int fd_ = 0;

        //状态 todo
        int state_ = 0;

        //数据cache
        std::vector<char> buf_;

        //buf最大长度
        int max_length_ = 4056;
        
        //buf当前位置
        int cur_pos_ = 0;

        //send cache
        std::vector<char> send_buf_;
};
#endif