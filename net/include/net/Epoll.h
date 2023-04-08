#pragma once
//封装一个Epoll类

#include <sys/epoll.h>
#include <vector>

class Epoll
{
public:
    Epoll();
    ~Epoll();

    bool add(int fd, uint32_t events);
    bool mod(int fd, uint32_t events); 
    bool del(int fd);
    int wait(int timeout = -1);
    struct epoll_event* getEvents() { return m_events.data(); }

private:
    int m_epollfd = -1;
    std::vector<struct epoll_event> m_events;
};

