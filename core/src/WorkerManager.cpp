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
            //�ӽ���

        sigset_t  set;      //�źż�
        sigemptyset(&set);  //����źż�
        if (sigprocmask(SIG_SETMASK, &set, NULL) == -1)  //ԭ����������10���źš���ֹfork()�ڼ��յ��źŵ��»��ҡ������ڲ��������κ��źš������κ��źš�
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
    sigemptyset(&set);   //����źż�

    //������Щ�ź���ִ�б������ڼ䲻ϣ���յ�
    sigaddset(&set, SIGCHLD);     //�ӽ���״̬�ı�
    sigaddset(&set, SIGALRM);     //��ʱ����ʱ
    sigaddset(&set, SIGIO);       //�첽I/O
    sigaddset(&set, SIGINT);      //�ն��жϷ�
    sigaddset(&set, SIGHUP);      //���ӶϿ�
    sigaddset(&set, SIGUSR1);     //�û������ź�
    sigaddset(&set, SIGUSR2);     //�û������ź�
    sigaddset(&set, SIGWINCH);    //�ն˴��ڴ�С�ı�
    sigaddset(&set, SIGTERM);     //��ֹ
    sigaddset(&set, SIGQUIT);     //�ն��˳���
    // .........���Ը��ݿ�����ʵ����Ҫ�������������Ҫ���ε��ź�......

    //  �����ź�;
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) //��һ����������SIG_BLOCK�������� ���� �µ��ź������� Ϊ ����ǰ�ź������� �� �ڶ�������ָ����źż��Ĳ���
    {        
        Logger::getInstance().Log(LogLevel::Error,"master process failed to block signal");
    }

    //���ý��̱���
    ProcessTitle title(g_os_argc,g_os_argv,environ);
    title.setProcTitle("dohna:master");

    //�������߳�
    for(int i = workerProcesses_;i>0;i--){
        // std::cout<<"creating worker"<<std::endl;
        createWorker();
    }

    sigemptyset(&set);  //����źż�
    if (sigprocmask(SIG_SETMASK, &set, NULL) == -1)  //ԭ����������10���źš���ֹfork()�ڼ��յ��źŵ��»��ҡ������ڲ��������κ��źš������κ��źš�
    {
        Logger::getInstance().Log(LogLevel::Error,"Master process failed to reset sigset");
    }


    //�����ź�
    SignalManager sm;
    sm.RegisterSignal(Signal(SIGINT,"SIGINT",[](int,siginfo_t *,void *){std::cout<<"Handling SIG_INT"<<std::endl;}));
    sm.RegisterSignal(Signal(SIGHUP,"SIGHUP",[](int,siginfo_t *,void *){std::cout<<"Handling SIG_INT"<<std::endl;}));

    sm.HandleSignals();



}


WorkerManager::~WorkerManager()
{
}
