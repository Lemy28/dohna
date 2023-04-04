#include <unistd.h>

//封装一个socket类


class Socket
{
    int m_sockfd = -1;
    int m_port = -1;

public:
    Socket() = default;
    ~Socket() { if (m_sockfd != -1) close(m_sockfd); }

    // 1. 创建socket
    bool CreateSocket();
    // 2. 绑定地址
    bool Bind(const char* ip, const int port);
    // 3. 监听
    bool Listen(const int backlog = 5);
    // 4. 接受连接
    int Accept(struct sockaddr_in* client_addr);
    // 5. 发送数据
    bool Send(const int connfd, const void* buf, int len);
    // 6. 接收数据
    int Recv(const int connfd, void* buf, int len);
    // 7. 关闭socket
    bool Close(const int connfd);

    int GetFd() const { return m_sockfd; }
    int GetPort() const { return m_port; }

    // 8. 设置socket为非阻塞
    bool SetNonBlocking(const int fd);
    // 9. 设置socket为阻塞
    bool SetBlocking(const int fd);

};

