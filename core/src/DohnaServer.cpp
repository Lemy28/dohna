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


    //(1)�����ػ����̵ĵ�һ����fork()һ���ӽ��̳���
    switch (fork())  //fork()��������ӽ��̲Ż��Ϊ�������ｲ���master���̣�
    {
    case -1:
        //�����ӽ���ʧ��
        Logger::getInstance()<<"failed to daemonize server";
        return -1;
    case 0:
        //�ӽ��̣��ߵ�����ֱ��break;
        break;
    default:
        //���������� ֱ���˳�exit(0);����ϣ���ص�������ȥ�ͷ�һЩ��Դ
        return 1;  //������ֱ�ӷ���1��
    } //end switch

    //ֻ��fork()�������ӽ��̲����ߵ��������
    m_ppid = m_pid;     //ngx_pid��ԭ�������̵�id����Ϊ�������ӽ��̣������ӽ��̵�ngx_parent����Ϊԭ�������̵�pid
    m_pid = getpid();       //��ǰ�ӽ��̵�idҪ����ȡ��
    
    //(2)�����նˣ��ն˹رգ��������ӽ����޹�
    if (setsid() < 0)  
    {
        Logger::getInstance()<<"failed to daemonize server";
        return -1;
    }

    //(3)����Ϊ0���������������ļ���Ŀ¼�������ж���д��ִ�е�Ȩ��
    umask(0); 

    //(4)�򿪺ڶ��豸���Զ�д��ʽ��
    int fd = open("/dev/null", O_RDWR);

    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

//    //Change the current working directory to root to avoid issues with unmounting file systems
//     if (chdir("/") < 0) {
//         exit(1);
//     }
    close(fd);//�ͷ���Դ��������ļ��������Ϳ��Ա����ã�
    
    return 0; //�ӽ��̷���0
}






DohnaServer::DohnaServer(){

    //������־
    Logger &logger = Logger::getInstance();
    logger.Log(LogLevel::Info,"Server started\n");

    //��������
    ConfigManager& cm = ConfigManager::getInstance();
    if(!cm.loadConfig("/etc/dohna.conf")){
        perror("ConfigManager:failed to load file:'/etc/dohna.conf' ");
        exit(-1);
    }
    //��Ϊ�ػ�����
    if(cm.getInt("daemon")){
        // std::cout<<"daemonized";
        switch (serverDaemonize())
        {
        case -1:
        //��̨��ʧ��
            exit(-1);
            break;
        case 0:
        //�ӽ���
        WorkerManager::getInstance().run();
            break;
        case 1:
        //�������˳�
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
