#pragma once
#include <unistd.h>

namespace dohna{

class DohnaServer
{
private:
    pid_t m_pid,m_ppid;
    int serverDaemonize();

public:
    DohnaServer();

    ~DohnaServer();

    void start();
    void stop();

};

}