#include <core/SignalManager.h>
#include <core/WorkerManager.h>
#include <tools/Logger.h>
#include <unistd.h>
#include <tools/ProcessTitle.h>
#include <sys/prctl.h>
#include <stdio.h>
#include <core/DohnaServerGlobal.h>




WorkerManager& WorkerManager::getInstance(){
    static WorkerManager wm;
    return wm;
}


void WorkerManager::createWorker()
 {
        pid_t pid = fork();
        if (pid == 0) {
            //子进程

        sigset_t  set;      //信号集
        sigemptyset(&set);  //清空信号集
        if (sigprocmask(SIG_SETMASK, &set, NULL) == -1)  //原来是屏蔽那10个信号【防止fork()期间收到信号导致混乱】，现在不再屏蔽任何信号【接收任何信号】
        {
            Logger::getInstance().Log(LogLevel::Error,"Child process failed to reset sigset with pid:%d",getpid());
        }
            ProcessTitle title(g_os_argc,g_os_argv,environ);
            title.setProcTitle("dohna:worker");

            ChildProcess childProcess;
            childProcess.run();
        }
        else if (pid > 0) {
            Logger &logger = Logger::getInstance();
        }
        else {
            Logger &logger = Logger::getInstance();
            logger.Log(LogLevel::Error,"Error creating child process\n");
        }
    }

WorkerManager::WorkerManager()
{
    ConfigManager &cm = ConfigManager::getInstance();
    if(!cm.loadConfig("/etc/dohna.conf")){
        perror("ConfigManager:failed to load file:'/etc/dohna.conf' ");
        exit(-1);
    }

    // std::cout<<"wokerCount:"<<cm.getInt("workerProcesses") <<std::endl;
    workerProcesses_ = cm.getInt("workerProcesses");

}

void WorkerManager::run(){

    sigset_t set;        
    sigemptyset(&set);   //清空信号集

    //下列这些信号在执行本函数期间不希望收到
    sigaddset(&set, SIGCHLD);     //子进程状态改变
    sigaddset(&set, SIGALRM);     //定时器超时
    sigaddset(&set, SIGIO);       //异步I/O
    sigaddset(&set, SIGINT);      //终端中断符
    sigaddset(&set, SIGHUP);      //连接断开
    sigaddset(&set, SIGUSR1);     //用户定义信号
    sigaddset(&set, SIGUSR2);     //用户定义信号
    sigaddset(&set, SIGWINCH);    //终端窗口大小改变
    sigaddset(&set, SIGTERM);     //终止
    sigaddset(&set, SIGQUIT);     //终端退出符
    // .........可以根据开发的实际需要往其中添加其他要屏蔽的信号......

    //  屏蔽信号;
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) //第一个参数用了SIG_BLOCK表明设置 进程 新的信号屏蔽字 为 “当前信号屏蔽字 和 第二个参数指向的信号集的并集
    {        
        Logger::getInstance().Log(LogLevel::Error,"master process failed to block signal");
    }

    //设置进程标题
    ProcessTitle title(g_os_argc,g_os_argv,environ);
    title.setProcTitle("dohna:master");

    //创建子线程
    for(int i = workerProcesses_;i>0;i--){
        // std::cout<<"creating worker"<<std::endl;
        createWorker();
    }

    sigemptyset(&set);  //清空信号集
    if (sigprocmask(SIG_SETMASK, &set, NULL) == -1)  //原来是屏蔽那10个信号【防止fork()期间收到信号导致混乱】，现在不再屏蔽任何信号【接收任何信号】
    {
        Logger::getInstance().Log(LogLevel::Error,"Master process failed to reset sigset");
    }


    //处理信号
    SignalManager sm;
    sm.RegisterSignal(Signal(SIGINT,"SIGINT",[](int,siginfo_t *,void *){std::cout<<"Handling SIG_INT"<<std::endl;}));
    sm.RegisterSignal(Signal(SIGHUP,"SIGHUP",[](int,siginfo_t *,void *){std::cout<<"Handling SIG_INT"<<std::endl;}));

    sm.HandleSignals();



}


WorkerManager::~WorkerManager()
{
}
