#include <core/SignalManager.h>
#include <core/WorkerManager.h>
#include <tools/Logger.h>
#include <unistd.h>
#include <tools/ProcessTitle.h>
#include <sys/prctl.h>



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
            Logger::getInstance().Log(LogLevel::Error,"Child process faied to reset sigset with pid:%d",getpid());
        }

            prctl(PR_SET_NAME,"dohna:worker process",0,0,0);

            ChildProcess childProcess;
            childProcess.run();
        }
        else if (pid > 0) {
            Logger &logger = Logger::getInstance();
            logger.Log(LogLevel::Info,"Created child process with pid: %d \n",getpid());
        }
        else {
            Logger &logger = Logger::getInstance();
            logger.Log(LogLevel::Error,"Error creating child process\n");
        }
    }

WorkerManager::WorkerManager()
{
    ConfigManager &cm = ConfigManager::getInstance();
    if(cm.loadConfig("dohna.conf")){
        Logger &logger = Logger::getInstance();
        logger.Log(LogLevel::Error,"Erorr loading config:dohna.conf");
    }

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
    //.........���Ը��ݿ�����ʵ����Ҫ�������������Ҫ���ε��ź�......

     //�����ź�;
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) //��һ����������SIG_BLOCK�������� ���� �µ��ź������� Ϊ ����ǰ�ź������� �� �ڶ�������ָ����źż��Ĳ���
    {        
        Logger::getInstance().Log(LogLevel::Error,"master process faied to block signal");
    }


    //�������߳�
    for(int i = workerProcesses_;i>0;i--){
        createWorker();
    }


    sleep(999999);

}


WorkerManager::~WorkerManager()
{
}
