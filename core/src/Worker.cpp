#include <core/Worker.h>
#include <tools/Logger.h>
#include <unistd.h>
#include <tools/ProcessTitle.h>
#include <signal.h>
#include <core/DohnaServerGlobal.h>
#include <net/Epoll.h>
#include <net/Socket.h>
#include <tools/ConfigManager.h>
#include <netinet/in.h>
namespace dohna{



Worker::Worker():m_port(0),m_epoll(),m_listenFd(socket(AF_INET,SOCK_STREAM,0))
{
    Logger &logger = Logger::getInstance();
    logger.Log(LogLevel::Info,"Worker process started with pid:%d",getpid());
    //���ý�����
    ProcessTitle title(g_os_argc,g_os_argv,environ);
    title.setProcTitle("dohna:worker");

    //��ȡ������Ϣ
    ConfigManager &cm = ConfigManager::getInstance();
    if(!cm.loadConfig("/etc/dohna.conf")){
        logger.LogError("ConfigManager:failed to load file:'/etc/dohna.conf' ");
        exit(-1);
    }
    m_port = cm.getInt("listenPort");



    //�����źŴ�����
    // signal(SIGINT,signalHandler);
    // signal(SIGTERM,signalHandler);
    // signal(SIGQUIT,signalHandler);
    // signal(SIGPIPE,SIG_IGN);

}

Worker::~Worker()
{

}


//��ʼ������socket
void Worker::init(){
    Socket ListenFd(m_listenFd);
    //���ü�����ַΪ������
    if(!ListenFd.setReuseAddr()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d set reuseaddr failed:",getpid(),strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d set reuseaddr success",getpid());
    }
    //���ü����˿�Ϊ������
    if(!ListenFd.setReusePort()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d set reuseport failed",getpid());
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d set reuseport success",getpid());
    }
    //���ü����׽���Ϊ�������Ƿ��б�Ҫ��
    //��Ϊepoll��ˮƽ�����ģ����������Ϊ����������ô���������ӵ���ʱ��epoll��һֱ������ֱ��accept�ɹ�
    //�������Ϊ����������ô���������ӵ���ʱ��epollֻ�ᴥ��һ�Σ�accept�ɹ���epoll�Ͳ����ٴ�����
    //������������Ϊ������
    // if(!ListenFd.setNonBlocking()){
    //     Logger::getInstance().Log(LogLevel::Error,"Worker %d set nonblocking failed",getpid());
    //     exit(-1);
    // }

    //����lingerѡ��
    //�������׽�������lingerѡ���б�Ҫ��
    //��Ϊ�����׽����Ƿ������ģ�������һ�㲻��رգ�������������lingerѡ��û�б�Ҫ
    // if(!ListenFd.setLinger()){
    //     Logger::getInstance().Log(LogLevel::Error,"Worker %d set linger failed",getpid());
    //     exit(-1);
    // }
    // else{
    //     Logger::getInstance().Log(LogLevel::Debug,"Worker %d set linger success",getpid());
    // }


    //�󶨶˿�
    if(!ListenFd.bind(m_port)){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d bind port:%d failed:%s",getpid(),m_port,strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d bind port:%d success",getpid(),m_port);
    }
    //��ʼ����
    if(!ListenFd.listen()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d listen failed:%s",getpid(),strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d listen success",getpid());
    }
    //�������׽��ּ���epoll
    if(!m_epoll.add(m_listenFd,EPOLLIN)){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d add listen fd to epoll failed:%s",getpid(),strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d add listen fd to epoll success",getpid());
    }
}

//ʹ��epoll������������������
void Worker::run(){

    int timeout = -1;
    init();
    Logger &logger = Logger::getInstance();
    while(true){

        timeout = m_timer.getNextTick();
        //epoll_wait
        int n = m_epoll.wait(timeout);
        //�����¼�
        for(int i = 0;i < n;i++){
            int fd = m_epoll.getEventFd(i);
            uint32_t events = m_epoll.getEvents(i);
            if(fd == m_listenFd){
                //����������
                if(events & EPOLLIN){
                    logger.Log(LogLevel::Debug,"Worker %d accept new connection",getpid());
                    acceptConnection();
                }
            }
            else{
                //���������ӵĿͻ���
                if(events & EPOLLIN){
                    logger.Log(LogLevel::Debug,"Worker %d handle client request",getpid());
                    handleClientRead(m_connections[fd]);
                }
                else if(events & EPOLLOUT){
                    logger.Log(LogLevel::Debug,"Worker %d handle client response",getpid());
                    handleClientWrite(m_connections[fd]);
                }
                else if(events & (EPOLLERR|EPOLLHUP|EPOLLRDHUP)) {
                    logger.Log(LogLevel::Debug,"Worker %d handle client error",getpid());
                    closeConnection(m_connections[fd]);
                }
                else{
                    logger.Log(LogLevel::Debug,"Worker %d handle client unknown",getpid());
                }
            }
        }
    }

}

//����ͻ��˵Ķ�����
void Worker::handleClientRead(HttpConnection &conn){
    //��ȡ�ͻ��˵�����
    //�������ýṹ���󶨣�����û�취������c++17������ֻ����pair��
    auto res = conn.recv();
    if(res.first < 0){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d read client request failed:%s",getpid(),strerror(errno));
        closeConnection(conn);
        return;
    }
    else if(res.first == 0){
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d client close connection",getpid());
        closeConnection(conn);
        return;
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d read client request success",getpid());
    }

    conn.handle();

    if(conn.isKeepAlive()){
        //����ǳ����ӣ���ô�ͼ����������¼�
        if(!m_epoll.mod(conn.getFd(),EPOLLIN)){
            Logger::getInstance().Log(LogLevel::Error,"Worker %d mod client fd to epoll failed:%s",getpid(),strerror(errno));
            closeConnection(conn);
            return;
        }
    }
    else{
        //����Ƕ����ӣ���ô�͹ر�����
        closeConnection(conn);
    }
 
}

//����ͻ��˵�д����
void Worker::handleClientWrite(HttpConnection &conn){
    return;
}

//����������
void Worker::acceptConnection(){
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientFd = accept(m_listenFd,(struct sockaddr*)&clientAddr,&clientAddrLen);
    if(clientFd < 0){
        return;
    }

    Socket clientSocket(clientFd);
    //���ÿͻ����׽��ֵ�lingerѡ��
    if(!clientSocket.setLinger()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d set client fd linger failed:%s",getpid(),strerror(errno));
        close(clientFd);
        return;
    }
    //���뵽epoll
    if(!m_epoll.add(clientFd,EPOLLIN)){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d add client fd to epoll failed:%s",getpid(),strerror(errno));
        close(clientFd);
        return;
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d add client fd to epoll success",getpid());
    }

    //���뵽���ӳ�
    m_connections[clientFd].init(clientFd,clientAddr);

}

//�ر�����
void Worker::closeConnection(HttpConnection &conn){
    m_epoll.del(conn.getFd());
    m_connections.erase(conn.getFd());
    conn.close();
}

}//end namespace dohna