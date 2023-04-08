//实现一个Worker类，作为一个进程处理网络请求
#pragma once
#include <net/Epoll.h>
#include <net/Socket.h> 
namespace dohna{


class Worker
{

private:

    Socket m_listenSocket;
    Epoll m_epoll;
    int m_listenPort;
public:
    Worker();
    ~Worker();

    void run();
};



}