#include <tools/Logger.h>
#include <tools/ConfigManager.h>
#include <tools/ProcessTitle.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <core/WorkerManager.h>
#include <core/DohnaServerGlobal.h>
#include <net/DohnaServer.h>

namespace dohna{

void DohnaServer::initEventMode(){
    //初始化事件驱动模式
    //1.获取配置文件中的事件驱动模式
    std::string eventMode = ConfigManager::getInstance().getString("eventMode");
    //2.根据配置文件中的事件驱动模式，初始化事件驱动模式
    if(eventMode == "LT"||"lt"){
        Logger::getInstance().LogInfo("server is running in LT mode");
        m_eventMode = EventMode::LT;

    }else{
        Logger::getInstance().LogInfo("server is running in ET mode");
        m_eventMode = EventMode::ET;
    }

    
}



int DohnaServer::daemonize(){

    Logger::getInstance().Log(LogLevel::Debug,"server is running in daemon mode");

    //(1)创建守护进程的第一步，fork()一个子进程出来
    switch (fork())
    {
    case -1:
        //创建子进程失败
        return -1;
    case 0:
        //子进程，走到这里直接break;
        break;
    default:
        return 1;  //父进程直接返回1；
    } //end switch

    //子进程才能走到这个流程
    m_ppid = m_pid;  
    m_pid = getpid();   
    
    //(2)脱离终端
    if (setsid() < 0)  
    {
        Logger::getInstance().LogError("failed to daemonize server");
        return -1;
    }

    //(3)设置为0，创建的所有新文件和目录都将具有读、写和执行的权限
    umask(0); 

    //(4)打开黑洞设备，以读写方式打开
    int fd = open("/dev/null", O_RDWR);

    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

   //(5)改变工作目录,避免文件系统被卸载导致的问题
    // if (chdir("/") < 0) {
    //     exit(EXIT_FAILURE);
    // }
    //但是这里不改变工作目录，因为我们的配置文件是在/etc/dohna.conf，如果改变了工作目录，就找不到配置文件了
    //所以我们在加载配置文件的时候，使用绝对路径
    close(fd);//释放资源这样这个文件描述符就可以被复用；
    
    return 0; //子进程返回0
}


DohnaServer::DohnaServer(){

    //载入日志
    Logger &logger = Logger::getInstance();
    logger.Log(LogLevel::Info,"Server started");

    //载入配置
    ConfigManager& cm = ConfigManager::getInstance();
    if(!cm.loadConfig("/etc/dohna.conf")){
        logger.LogError("failed to load config file");
        exit(EXIT_FAILURE);
    }
    else{
        logger.Log(LogLevel::Debug,"config file loaded");
    }
    //变为守护进程
    if(cm.getInt("daemon")){
        switch (daemonize())
        {
        case -1:
        //后台化失败
            Logger::getInstance().LogError("failed to daemonize server");
            exit(EXIT_FAILURE);
            break;
        case 0:
        //执行主要任务
            logger.Log(LogLevel::Info,"Server is running in background");
            WorkerManager::getInstance().run();
            break;
        case 1:
        //父进程退出
            exit(0);
            break;
        
        default:
            break;
        }//end switch

        
    }
    else{
        //不需要变为守护进程
        logger.Log(LogLevel::Info,"Server is running in foreground");
        WorkerManager::getInstance().run();
    }

}

DohnaServer::~DohnaServer()
{
    Logger::getInstance().LogInfo("Server stopped");
}


}