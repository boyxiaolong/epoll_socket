# epoll_socket
实现了简单的基于epoll的服务端

编译：需要安装scons，protobuf_complier,然后cd epoll_socket,执行scons即可,

暂时只支持一个epoll一个Server，监听一个端口，处理多个客户端连接;

可以定制自己的server，client，具体实例在/logic内，实现了简单的protobuf协议的server client.

TODO:

1. API整理
2. 加入protobuf协议支持
