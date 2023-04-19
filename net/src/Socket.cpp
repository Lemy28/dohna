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
//�󶨵�ַ
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




//����
bool Socket::listen(const int backlog)
{
    if (::listen(m_sockfd, backlog) == -1)
    {
        return false;
    }
    return true;
}
//��������
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
//���õ�ַ����
bool Socket::setReuseAddr()
{
    int opt = 1;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;
}
//���ö˿ڸ���
bool Socket::setReusePort()
{
    int opt = 1;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;
}
//�ر�����
bool Socket::close()
{
    if (::close(m_sockfd) == -1)
    {
        return false;
    }
    return true;
}

//����keepalive
bool Socket::setKeepAlive()
{
    int opt = 1;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;
}

//����nodelay
bool Socket::setTcpNoDelay()
{
    int opt = 1;
    if(setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;

}

//����linger,��socket�ر�ʱ�����socket�������������ݣ���ȴ�һ��ʱ�䣬ֱ�����ݷ������
bool Socket::setLinger()
{
    struct linger opt;
    opt.l_onoff = 1;

    //�ȴ�ʱ��Ϊ1��
    opt.l_linger = 1;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt))<0)
    {
        return false;
    }
    return true;
}


}