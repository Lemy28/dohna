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
    //��ʼ���¼�����ģʽ
    //1.��ȡ�����ļ��е��¼�����ģʽ
    std::string eventMode = ConfigManager::getInstance().getString("eventMode");
    //2.���������ļ��е��¼�����ģʽ����ʼ���¼�����ģʽ
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

    //(1)�����ػ����̵ĵ�һ����fork()һ���ӽ��̳���
    switch (fork())
    {
    case -1:
        //�����ӽ���ʧ��
        return -1;
    case 0:
        //�ӽ��̣��ߵ�����ֱ��break;
        break;
    default:
        return 1;  //������ֱ�ӷ���1��
    } //end switch

    //�ӽ��̲����ߵ��������
    m_ppid = m_pid;  
    m_pid = getpid();   
    
    //(2)�����ն�
    if (setsid() < 0)  
    {
        Logger::getInstance().LogError("failed to daemonize server");
        return -1;
    }

    //(3)����Ϊ0���������������ļ���Ŀ¼�������ж���д��ִ�е�Ȩ��
    umask(0); 

    //(4)�򿪺ڶ��豸���Զ�д��ʽ��
    int fd = open("/dev/null", O_RDWR);

    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

   //(5)�ı乤��Ŀ¼,�����ļ�ϵͳ��ж�ص��µ�����
    // if (chdir("/") < 0) {
    //     exit(EXIT_FAILURE);
    // }
    //�������ﲻ�ı乤��Ŀ¼����Ϊ���ǵ������ļ�����/etc/dohna.conf������ı��˹���Ŀ¼�����Ҳ��������ļ���
    //���������ڼ��������ļ���ʱ��ʹ�þ���·��
    close(fd);//�ͷ���Դ��������ļ��������Ϳ��Ա����ã�
    
    return 0; //�ӽ��̷���0
}


DohnaServer::DohnaServer(){

    //������־
    Logger &logger = Logger::getInstance();
    logger.Log(LogLevel::Info,"Server started");

    //��������
    ConfigManager& cm = ConfigManager::getInstance();
    if(!cm.loadConfig("/etc/dohna.conf")){
        logger.LogError("failed to load config file");
        exit(EXIT_FAILURE);
    }
    else{
        logger.Log(LogLevel::Debug,"config file loaded");
    }
    //��Ϊ�ػ�����
    if(cm.getInt("daemon")){
        switch (daemonize())
        {
        case -1:
        //��̨��ʧ��
            Logger::getInstance().LogError("failed to daemonize server");
            exit(EXIT_FAILURE);
            break;
        case 0:
        //ִ����Ҫ����
            logger.Log(LogLevel::Info,"Server is running in background");
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
        //����Ҫ��Ϊ�ػ�����
        logger.Log(LogLevel::Info,"Server is running in foreground");
        WorkerManager::getInstance().run();
    }

}

DohnaServer::~DohnaServer()
{
    Logger::getInstance().LogInfo("Server stopped");
}


}