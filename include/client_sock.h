#ifndef _client_sock_h__
#define _client_sock_h__

#include <stdint.h>

#include <vector>
#include <queue>
#include <memory>

#include "net_buffer.h"

enum socket_state {
    socket_create,
    socket_connecting,
    socket_connected,
    socket_close
};

typedef std::queue<std::shared_ptr<net_buffer> > net_buffer_vec;

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
        int set_event(bool is_read, bool is_write);

        //同步连接
        int sync_connect(const char* ip, uint16_t port);

        //发送数据
        int send_data(char* pdata, int length);

        int send_data(std::shared_ptr<net_buffer> buff_data);

        int set_timeout();

        int set_keeplive();

        virtual void update();

        virtual void on_disconnect();

        int get_state(){return state_;}

        bool is_block() {return is_block_;}

        int on_read();

        int on_write();

        virtual void before_heart_beat();

        virtual void on_heart_beat();

    protected:
        void add_pos(int length);

        //buf扩展
        void expand_buf();

    private:
        int _send_data();

        void _heart_beat_check(int64_t cur);

        int _start_heart_beat();

    protected:
        //epoll fd
        int ae_fd_ = 0;

        //本socket fd
        int fd_ = 0;

        //状态 todo
        int state_ = socket_create;

        //数据cache
        net_buffer buf_;

        //buf最大长度
        int max_length_ = 4056;
        
        //buf当前位置
        int cur_pos_ = 0;

        bool is_block_ = true;

        net_buffer_vec read_net_buffer_vec_;

        net_buffer_vec send_net_buffer_vec_;
        
        bool is_sending_ = false;

        int ep_event_ = 0; 

        int heart_beat_interval_ = 1000;

        int heart_beat_timer_id_ = 0;
};
#endif