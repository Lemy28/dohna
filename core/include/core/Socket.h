#include <unistd.h>

//��װһ��socket��


class Socket
{
    int m_sockfd = -1;
    int m_port = -1;

public:
    Socket() = default;
    ~Socket() { if (m_sockfd != -1) close(m_sockfd); }

    // 1. ����socket
    bool CreateSocket();
    // 2. �󶨵�ַ
    bool Bind(const char* ip, const int port);
    // 3. ����
    bool Listen(const int backlog = 5);
    // 4. ��������
    int Accept(struct sockaddr_in* client_addr);
    // 5. ��������
    bool Send(const int connfd, const void* buf, int len);
    // 6. ��������
    int Recv(const int connfd, void* buf, int len);
    // 7. �ر�socket
    bool Close(const int connfd);

    int GetFd() const { return m_sockfd; }
    int GetPort() const { return m_port; }

    // 8. ����socketΪ������
    bool SetNonBlocking(const int fd);
    // 9. ����socketΪ����
    bool SetBlocking(const int fd);

};

