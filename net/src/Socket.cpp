#include <unistd.h>
#include <net/Socket.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tools/Logger.h>
namespace dohna{

Socket::Socket(){ 
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
}
//绑定地址
bool Socket::bind(const char* ip, const int port)
{
    struct ::sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    if (::bind(m_sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        return false;
    }
    return true;
}
bool Socket::bind(const int port)
{
    struct ::sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(m_sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        return false;
    }
    return true;
}




//监听
bool Socket::listen(const int backlog)
{
    if (::listen(m_sockfd, backlog) == -1)
    {
        return false;
    }
    return true;
}
//接受连接
int Socket::accept(struct ::sockaddr_in* client_addr)
{
    socklen_t client_addr_len = sizeof(struct ::sockaddr_in);
    int connfd = ::accept(m_sockfd, (struct sockaddr*) client_addr, &client_addr_len);
    if (connfd == -1)
    {
        return -1;
    }
    return connfd;
}
//发送数据
bool Socket::sendData(const void* buf, int len)
{
    int send_len = 0;
    while (send_len < len)
    {
        int ret = send(m_sockfd, (char*) buf + send_len, len - send_len, 0);
        if (ret == -1)
        {
            return false;
        }
        send_len += ret;
    }
    return true;
}
//接收数据
int Socket::recvData(void* buf, int len)
{
    int recv_len = 0;
    while (recv_len < len)
    {
        int ret = recv(m_sockfd, (char*) buf + recv_len, len - recv_len, 0);
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

bool Socket::setNonBlocking()
{
    int old_option = fcntl(m_sockfd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(m_sockfd, F_SETFL, new_option);
    return old_option & O_NONBLOCK;
}
bool Socket::setBlocking()
{
    int old_option = fcntl(m_sockfd, F_GETFL);
    int new_option = old_option & ~O_NONBLOCK;
    fcntl(m_sockfd, F_SETFL, new_option);
    return old_option & O_NONBLOCK;
}
//设置地址复用
bool Socket::setReuseAddr()
{
    int opt = 1;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;
}
//设置端口复用
bool Socket::setReusePort()
{
    int opt = 1;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;
}

}