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
#include <map>


class client_sock
{
    public:
        client_sock(int fd):fd_(fd)
        , max_length(1024)
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
                    
                    is_read_error = true;
                    break;
                }
                if (nread == 0)
                {
                    if (readn == 0)
                    {
                        is_read_error = true;
                    }
                    else
                    {
                        printf("readnum 0 so read end");
                    }
                    
                    break;
                }
                
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
        }

        void close_sock(){
            close(fd_);
        }

        int get_fd(){
            return fd_;
        }

    private:
        int fd_;
        int state_;
        char* buf_;
        int max_length;
        int cur_pos;
};
#endif