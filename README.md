# epoll_socket
实现了简单的基于epoll的服务端

编译：需要安装scons，然后cd epoll_socket,执行scons即可

暂时只支持一个epoll一个Server，监听一个端口，处理多个客户端连接

TODO:

1. API整理
2. 加入protobuf协议支持
