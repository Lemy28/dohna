#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

//封装一个socket类
namespace dohna{

class Socket
{
    int m_sockfd = -1;

public:
    Socket();
    Socket(int sockfd) : m_sockfd(sockfd) {}
    ~Socket() = default;

    int getFd() const { return m_sockfd; }

    bool bind(const char* ip, const int port);
    bool bind(const int port);
    bool listen(const int backlog = 5);
    int accept(struct sockaddr_in* client_addr);
    bool close();


    bool setNonBlocking();
    bool setBlocking();
    bool setReuseAddr();
    bool setReusePort();
    bool setKeepAlive();
    bool setTcpNoDelay();
    bool setLinger();
};

}