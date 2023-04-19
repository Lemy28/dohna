#pragma once
#include <unistd.h>

namespace dohna{

enum class EventMode
{
    ET,
    LT
};

class DohnaServer
{
private:
    pid_t m_pid,m_ppid;
    EventMode m_eventMode;

    //守护进程化
    int daemonize();
    bool isDaemon = false;


    //初始化事件驱动模式
    void initEventMode();

public:
    DohnaServer();
    ~DohnaServer();

    bool isDaemonized() const { return isDaemon; }
    EventMode getEventMode()const;

};

}