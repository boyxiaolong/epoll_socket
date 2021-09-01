#ifndef _client_sock_h__
#define _client_sock_h__

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

    private:
        void add_pos(int length);

    private:
        int ae_fd_;
        int fd_;
        int state_;
        char* buf_;
        int max_length;
        int cur_pos;
};
#endif