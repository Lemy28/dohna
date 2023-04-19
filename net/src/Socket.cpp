#include <unistd.h>
#include <net/Socket.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tools/Logger.h>
//tcpnoDelay
#include <netinet/tcp.h>

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
//关闭连接
bool Socket::close()
{
    if (::close(m_sockfd) == -1)
    {
        return false;
    }
    return true;
}

//设置keepalive
bool Socket::setKeepAlive()
{
    int opt = 1;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;
}

//设置nodelay
bool Socket::setTcpNoDelay()
{
    int opt = 1;
    if(setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;

}

//设置linger,当socket关闭时，如果socket缓冲区还有数据，会等待一段时间，直到数据发送完毕
bool Socket::setLinger()
{
    struct linger opt;
    opt.l_onoff = 1;

    //等待时间为1秒
    opt.l_linger = 1;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;
}


}