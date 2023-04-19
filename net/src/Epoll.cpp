#include <net/Epoll.h>
#include <net/Socket.h>
#include <tools/Logger.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

Epoll::Epoll()
{   
    m_epollfd = epoll_create1(EPOLL_CLOEXEC);
    if (m_epollfd == -1)
    {
        Logger::getInstance().LogError("epoll_create1 error");
        exit(-1);
    }

    m_events.resize(1024);


}

Epoll::~Epoll()
{
    close(m_epollfd);
}

//添加事件
bool Epoll::add(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        Logger::getInstance().LogError("epoll_ctl error");
        return false;
    }
    return true;
}

//修改事件
bool Epoll::mod(int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        Logger::getInstance().LogError("epoll_ctl error");
        return false;
    }
    return true;
}

//删除事件
bool Epoll::del(int fd)
{
    if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, NULL) == -1)
    {
        Logger::getInstance().LogError("epoll_ctl error");
        return false;
    }
    return true;
}

int Epoll::wait(int timeout)
{   
    int ret = epoll_wait(m_epollfd, m_events.data(), m_events.size(), timeout);
    if (ret == -1)
    {
        Logger::getInstance().Log(LogLevel::Error,"epoll_wait error:%s",std::strerror(errno));
        exit(EXIT_FAILURE);
        return -1;
    }
    if (ret == m_events.size())
    {
        m_events.resize(m_events.size() * 2);
    }
    return ret;
}
