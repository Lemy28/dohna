#include <unistd.h>
#include <memory>
#include <tools/Logger.h>
#include <core/SignalManager.h>
#include <signal.h>
#include <sys/prctl.h>
#include <core/DohnaServer.h>


//ȫ�ֱ����������޸Ľ�����
char* gp_envmem = nullptr;
char** g_os_argv;
int g_os_argc;
int g_environlen = 0;

pid_t dn_pid;

using namespace dohna;

int main(const int argc,char* *argv){


    g_os_argc = argc;           //�����������
    g_os_argv = (char **) argv; //�������ָ��

    DohnaServer();
 
    // SignalManager sm;
    // sm.RegisterSignal(Signal(SIGINT,"SIGINT",[](int,siginfo_t *,void *){std::cout<<"Handling SIG_INT"<<std::endl;}));
    // sm.HandleSignals();

    return 0;
    


}