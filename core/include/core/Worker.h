//ʵ��һ��Worker�࣬��Ϊһ�����̴�����������
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
    
    //��ʼ������socket
    void init();
    //����������
    void acceptConnection();
    //�ر�����
    void closeConnection(HttpConnection &conn);
    //����ͻ���д�¼�
    void handleClientWrite(HttpConnection &conn);
    //����ͻ��˶��¼�
    void handleClientRead(HttpConnection &conn);


    int m_listenFd;
    int m_port;
    int m_timeOut;//����Ϊ��λ

    HeapTimer m_timer;
    std::unordered_map<int,HttpConnection> m_connections;
    Epoll m_epoll;

};



}