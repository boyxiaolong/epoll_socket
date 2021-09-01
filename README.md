# epoll_socket
实现了简单的基于epoll的服务端

编译：需要安装scons，protobuf_complier,然后cd epoll_socket,执行scons即可,

暂时只支持一个epoll一个Server，监听一个端口，处理多个客户端连接;

可以定制自己的server，client，具体实例在/logic内，实现了简单的protobuf协议的server client.

编译好后，可执行sh mem_check.sh 检测是否有内存泄漏.

执行 ./epoll_test 默认开启端口为9999的服务器

./epoll_test 1 10 启动10个线程，每个线程启动1个客户端去连接服务器。

TODO:

1. 超时，心跳
2. tcp消息(比如游戏中玩家消息)分包后，可以投递到work_thread去处理. 
