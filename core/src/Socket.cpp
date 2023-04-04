#include <unistd.h>
#include <core/Socket.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <tools/Logger.h>
//����ͷ�ļ�ʵ��һ��socket��


// 1. ����socket
bool Socket::CreateSocket()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1)
    {
        return false;
    }
    return true;
}
//2. �󶨵�ַ
bool Socket::Bind(const char* ip, const int port)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    if (bind(m_sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        return false;
    }
    m_port = port;
    return true;
}
// 3. ����
bool Socket::Listen(const int backlog)
{
    if (listen(m_sockfd, backlog) == -1)
    {
        return false;
    }
    return true;
}
// 4. ��������
int Socket::Accept(struct sockaddr_in* client_addr)
{
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    int connfd = accept(m_sockfd, (struct sockaddr*) client_addr, &client_addr_len);
    if (connfd == -1)
    {
        return -1;
    }
    return connfd;
}
// 5. ��������
bool Socket::Send(const int connfd, const void* buf, int len)
{
    int send_len = 0;
    while (send_len < len)
    {
        int ret = send(connfd, (char*) buf + send_len, len - send_len, 0);
        if (ret == -1)
        {
            return false;
        }
        send_len += ret;
    }
    return true;
}
// 6. ��������
int Socket::Recv(const int connfd, void* buf, int len)
{
    int recv_len = 0;
    while (recv_len < len)
    {
        int ret = recv(connfd, (char*) buf + recv_len, len - recv_len, 0);
        if (ret == -1)
        {
            return -1;
        }
        else if (ret == 0)
        {
            return 0;
        }
        recv_len += ret;
    }
    return recv_len;
}
// 7. �ر�socket
bool Socket::Close(const int connfd)
{
    if (close(connfd) == -1)
    {
        return false;
    }
    return true;
}
// 8. ����socketΪ������
bool Socket::SetNonBlocking(const int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option & O_NONBLOCK;
}
// 9. ����socketΪ����
bool Socket::SetBlocking(const int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option & ~O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option & O_NONBLOCK;
}