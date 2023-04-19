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

    //�ػ����̻�
    int daemonize();
    bool isDaemon = false;


    //��ʼ���¼�����ģʽ
    void initEventMode();

public:
    DohnaServer();
    ~DohnaServer();

    bool isDaemonized() const { return isDaemon; }
    EventMode getEventMode()const;

};

}