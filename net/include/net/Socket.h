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

    // 2. 绑定地址
    bool bind(const char* ip, const int port);
    bool bind(const int port);
    // 3. 监听
    bool listen(const int backlog = 5);
    // 4. 接受连接
    int accept(struct sockaddr_in* client_addr);
    // 5. 发送数据
    bool sendData(const void* buf, int len);
    // 6. 接收数据
    int recvData(void* buf, int len);
    // 7. 关闭连接
    bool close();

    bool setNonBlocking();
    bool setBlocking();
    bool setReuseAddr();
    bool setReusePort();
};

}