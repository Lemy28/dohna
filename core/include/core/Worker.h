//ʵ��һ��Worker�࣬��Ϊһ�����̴�����������
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