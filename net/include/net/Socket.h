#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

//��װһ��socket��
namespace dohna{

class Socket
{
    int m_sockfd = -1;

public:
    Socket();
    Socket(int sockfd) : m_sockfd(sockfd) {}
    ~Socket() = default;

    int getFd() const { return m_sockfd; }

    // 2. �󶨵�ַ
    bool bind(const char* ip, const int port);
    bool bind(const int port);
    // 3. ����
    bool listen(const int backlog = 5);
    // 4. ��������
    int accept(struct sockaddr_in* client_addr);
    // 5. ��������
    bool sendData(const void* buf, int len);
    // 6. ��������
    int recvData(void* buf, int len);
    // 7. �ر�����
    bool close();

    bool setNonBlocking();
    bool setBlocking();
    bool setReuseAddr();
    bool setReusePort();
};

}