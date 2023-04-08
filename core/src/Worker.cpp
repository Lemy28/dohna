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



Worker::Worker():m_listenPort(0),m_epoll(),m_listenSocket()
{
    Logger &logger = Logger::getInstance();
    logger.Log(LogLevel::Info,"Worker process started with pid:%d",getpid());
    //设置进程名
    ProcessTitle title(g_os_argc,g_os_argv,environ);
    title.setProcTitle("dohna:worker");

    //读取配置信息
    ConfigManager &cm = ConfigManager::getInstance();
    if(!cm.loadConfig("/etc/dohna.conf")){
        logger.LogError("ConfigManager:failed to load file:'/etc/dohna.conf' ");
        exit(-1);
    }
    m_listenPort = cm.getInt("listenPort");
    //设置信号处理函数
    // signal(SIGINT,signalHandler);
    // signal(SIGTERM,signalHandler);
    // signal(SIGQUIT,signalHandler);
    // signal(SIGPIPE,SIG_IGN);

}

Worker::~Worker()
{

}
//使用epoll来处理网络连接请求
void Worker::run(){

    if(m_listenPort == 0){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d listen port is 0",getpid());
        exit(-1);
    }

    //设置监听地址为可重用
    if(!m_listenSocket.setReuseAddr()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d set reuseaddr failed:",getpid(),strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d set reuseaddr success",getpid());
    }
    //设置监听端口为可重用
    if(!m_listenSocket.setReusePort()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d set reuseport failed",getpid());
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d set reuseport success",getpid());
    }


    // //设置监听套接字为非阻塞
    // if(!m_listenSocket.setNonBlocking()){
    //     Logger::getInstance().Log(LogLevel::Error,"Worker %d set nonblocking failed",getpid());
    //     exit(-1);
    // }


    //绑定端口
    if(!m_listenSocket.bind(m_listenPort)){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d bind port:%d failed:%s",getpid(),m_listenPort,strerror(errno));
        exit(EXIT_FAILURE);
    }
    else{
        Logger::getInstance().Log(LogLevel::Info,"Worker %d bind port:%d success",getpid(),m_listenPort);
    }
    //监听端口
    if(!m_listenSocket.listen()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d listen port:%d failed",getpid(),m_listenPort);
        exit(EXIT_FAILURE);
    }else{
        Logger::getInstance().Log(LogLevel::Info,"Worker %d is listening port:%d",getpid(),m_listenPort);
    }

    //将监听套接字加入epoll
    if(!m_epoll.add(m_listenSocket.getFd(),EPOLLIN)){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d add listen socket to epoll failed",getpid());
        exit(EXIT_FAILURE);
    }else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d add listen socket to epoll success",getpid());
    }

    //使用epoll来处理网络连接请求
    while(true){
        int count =  m_epoll.wait();
        for(int i = 0;i < count;i++){
            if(m_epoll.getEvents()[i].data.fd == m_listenSocket.getFd()){
                //有新的连接请求,准备一个客户端地址结构体
                struct ::sockaddr_in clientAddr;
                Socket clientFd = m_listenSocket.accept((struct sockaddr_in*)&clientAddr);
                if(clientFd.getFd() == -1){
                    Logger::getInstance().Log(LogLevel::Error,"accept error");
                    continue;
                }
                //将客户端套接字加入epoll
                m_epoll.add(clientFd.getFd(),EPOLLIN);
                //
                clientFd.sendData("hello",5);


            }
            else{
                //有数据到来
                Socket clientFd = m_epoll.getEvents()[i].data.fd;
                char buf[1024];
                int n = clientFd.recvData(buf,1024);
                if(n == 0){
                    //客户端关闭连接
                    m_epoll.del(clientFd.getFd());
                    close(clientFd.getFd());
                }
                else if(n == -1){
                    Logger::getInstance().Log(LogLevel::Error,"worker process %d:read error",getpid());
                    m_epoll.del(clientFd.getFd());
                    close(clientFd.getFd());
                }
                else{
                    //将数据写回客户端
                    clientFd.sendData(buf,n);
                }
            }
        }

    }

}

}//end namespace dohna