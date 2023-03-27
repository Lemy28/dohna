#include <core/DohnaServer.h>
#include <tools/Logger.h>
#include <tools/ConfigManager.h>
#include <tools/ProcessTitle.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <core/WorkerManager.h>
#include <core/DohnaServerGlobal.h>


int DohnaServer::serverDaemonize(){


    //(1)创建守护进程的第一步，fork()一个子进程出来
    switch (fork())  //fork()出来这个子进程才会成为咱们这里讲解的master进程；
    {
    case -1:
        //创建子进程失败
        Logger::getInstance()<<"failed to daemonize server";
        return -1;
    case 0:
        //子进程，走到这里直接break;
        break;
    default:
        //父进程以往 直接退出exit(0);现在希望回到主流程去释放一些资源
        return 1;  //父进程直接返回1；
    } //end switch

    //只有fork()出来的子进程才能走到这个流程
    m_ppid = m_pid;     //ngx_pid是原来父进程的id，因为这里是子进程，所以子进程的ngx_parent设置为原来父进程的pid
    m_pid = getpid();       //当前子进程的id要重新取得
    
    //(2)脱离终端，终端关闭，将跟此子进程无关
    if (setsid() < 0)  
    {
        Logger::getInstance()<<"failed to daemonize server";
        return -1;
    }

    //(3)设置为0，创建的所有新文件和目录都将具有读、写和执行的权限
    umask(0); 

    //(4)打开黑洞设备，以读写方式打开
    int fd = open("/dev/null", O_RDWR);

    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

//    //Change the current working directory to root to avoid issues with unmounting file systems
//     if (chdir("/") < 0) {
//         exit(1);
//     }
    close(fd);//释放资源这样这个文件描述符就可以被复用；
    
    return 0; //子进程返回0
}






DohnaServer::DohnaServer(){

    //载入日志
    Logger &logger = Logger::getInstance();
    logger.Log(LogLevel::Info,"Server started\n");

    //载入配置
    ConfigManager& cm = ConfigManager::getInstance();
    if(!cm.loadConfig("/etc/dohna.conf")){
        perror("ConfigManager:failed to load file:'/etc/dohna.conf' ");
        exit(-1);
    }
    //变为守护进程
    if(cm.getInt("daemon")){
        // std::cout<<"daemonized";
        switch (serverDaemonize())
        {
        case -1:
        //后台化失败
            exit(-1);
            break;
        case 0:
        //子进程
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
        logger.Log(LogLevel::Error,"failed to daemonize");
    }

    

}

DohnaServer::~DohnaServer()
{
}
