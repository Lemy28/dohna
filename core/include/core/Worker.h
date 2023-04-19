//实现一个Worker类，作为一个进程处理网络请求
#pragma once
#include <net/Epoll.h>
#include <net/Socket.h> 
#include <net/HttpConnection.h>
#include <tools/HeapTimer.h>
#include <memory>
#include <unordered_map>

namespace dohna{


class Worker
{


public:
    Worker();
    ~Worker();

    void run();

private:
    void signalHandler(int sig);
    
    //初始化监听socket
    void init();
    //处理新连接
    void acceptConnection();
    //关闭连接
    void closeConnection(HttpConnection &conn);
    //处理客户端写事件
    void handleClientWrite(HttpConnection &conn);
    //处理客户端读事件
    void handleClientRead(HttpConnection &conn);


    int m_listenFd;
    int m_port;
    int m_timeOut;//毫秒为单位

    HeapTimer m_timer;
    std::unordered_map<int,HttpConnection> m_connections;
    Epoll m_epoll;

};



}