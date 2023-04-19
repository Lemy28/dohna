#pragma once

#include <tools/Buffer.h>
#include <net/Socket.h>
#include <net/HttpRequest.h>
#include <net/HttpResponse.h>

namespace dohna{


//��Socket��Ļ����Ϸ�װһ��HttpConnection��

class HttpConnection
{

public:
    HttpConnection(int clientFd,sockaddr_in clientAddr);
    HttpConnection() = default;
    ~HttpConnection();

    void init(int clientFd,sockaddr_in clientAddr);
    //��ȡsocket��fd
    int getFd() { return m_socket.getFd(); }
    //�ر�socket
    void close() { m_socket.close(); m_isClosed = true; }
    //�ж�socket�Ƿ�ر�
    bool isClosed() { return m_isClosed; }
    //�ж��Ƿ��ǳ�����
    bool isKeepAlive() { return m_request.isKeepAlive(); }

    //��ȡ����
    std::pair<int,bool> recv();
    //д������
    std::pair<int,bool> send();
    //����http����
    bool handle();
    
    

private:


    sockaddr_in m_clientAddr;
    Socket m_socket;
    bool m_isClosed = false;

    Buffer m_inputBuffer;
    Buffer m_outputBuffer;

    HttpRequest m_request;
    HttpResponse m_response;



};





}