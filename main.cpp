#include <unistd.h>
#include <memory>
#include <tools/ConfigManager.h>
#include <tools/Logger.h>
#include <tools/ProcessTitle.h>
#include <core/SignalManager.h>
#include <signal.h>
#include <core/WorkerManager.h>
#include <sys/prctl.h>



char* gp_envmem = nullptr;
char** g_os_argv;
int g_environlen = 0;

pid_t dn_pid;

int main(const int argc,char* *argv){

    int exitcode = 0;

    dn_pid = getpid();
    g_os_argv = (char**)argv;

    Logger &logger = Logger::getInstance();
    logger.Log(LogLevel::Info,"Server started\n");

    ConfigManager &configmanager = ConfigManager::getInstance();
    if(configmanager.loadConfig("/mnt/e/dohna/dohna.conf") == false){
        logger.Log(LogLevel::Error,"failed to load config");
        exitcode=2;
        return 1;
    }

    prctl(PR_SET_NAME,"dohna:master");

    // ProcessTitle title(argc,argv,environ);
    // title.setProcTitle("dohna:master");

    // SignalManager sm;
    // sm.RegisterSignal(Signal(SIGINT,"SIGINT",[](int,siginfo_t *,void *){std::cout<<"Handling SIG_INT"<<std::endl;}));
    // sm.HandleSignals();

    //创建子线程
    //WorkerManager::getInstance().run;

    // SignalManager sm;
    // sm.RegisterSignal(Signal(SIGINT,"SIGINT",[](int,siginfo_t *,void *){std::cout<<"Handling SIG_INT"<<std::endl;}));
    // sm.HandleSignals();




    std::cout<<"bye"<<std::endl;

    return 0;
    


}