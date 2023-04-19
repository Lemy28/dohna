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
    //设置进程名
    ProcessTitle title(g_os_argc,g_os_argv,environ);
    title.setProcTitle("dohna:worker");

    //读取配置信息
    ConfigManager &cm = ConfigManager::getInstance();
    if(!cm.loadConfig("/etc/dohna.conf")){
        logger.LogError("ConfigManager:failed to load file:'/etc/dohna.conf' ");
        exit(-1);
    }
    m_port = cm.getInt("listenPort");



    //设置信号处理函数
    // signal(SIGINT,signalHandler);
    // signal(SIGTERM,signalHandler);
    // signal(SIGQUIT,signalHandler);
    // signal(SIGPIPE,SIG_IGN);

}

Worker::~Worker()
{

}


//初始化监听socket
void Worker::init(){
    Socket ListenFd(m_listenFd);
    //设置监听地址为可重用
    if(!ListenFd.setReuseAddr()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d set reuseaddr failed:",getpid(),strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d set reuseaddr success",getpid());
    }
    //设置监听端口为可重用
    if(!ListenFd.setReusePort()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d set reuseport failed",getpid());
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d set reuseport success",getpid());
    }
    //设置监听套接字为非阻塞是否有必要？
    //因为epoll是水平触发的，如果不设置为非阻塞，那么当有新连接到来时，epoll会一直触发，直到accept成功
    //如果设置为非阻塞，那么当有新连接到来时，epoll只会触发一次，accept成功后，epoll就不会再触发了
    //所以这里设置为非阻塞
    // if(!ListenFd.setNonBlocking()){
    //     Logger::getInstance().Log(LogLevel::Error,"Worker %d set nonblocking failed",getpid());
    //     exit(-1);
    // }

    //设置linger选项
    //给监听套接字设置linger选项有必要吗？
    //因为监听套接字是服务器的，服务器一般不会关闭，所以这里设置linger选项没有必要
    // if(!ListenFd.setLinger()){
    //     Logger::getInstance().Log(LogLevel::Error,"Worker %d set linger failed",getpid());
    //     exit(-1);
    // }
    // else{
    //     Logger::getInstance().Log(LogLevel::Debug,"Worker %d set linger success",getpid());
    // }


    //绑定端口
    if(!ListenFd.bind(m_port)){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d bind port:%d failed:%s",getpid(),m_port,strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d bind port:%d success",getpid(),m_port);
    }
    //开始监听
    if(!ListenFd.listen()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d listen failed:%s",getpid(),strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d listen success",getpid());
    }
    //将监听套接字加入epoll
    if(!m_epoll.add(m_listenFd,EPOLLIN)){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d add listen fd to epoll failed:%s",getpid(),strerror(errno));
        exit(-1);
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d add listen fd to epoll success",getpid());
    }
}

//使用epoll来处理网络连接请求
void Worker::run(){

    int timeout = -1;
    init();
    Logger &logger = Logger::getInstance();
    while(true){

        timeout = m_timer.getNextTick();
        //epoll_wait
        int n = m_epoll.wait(timeout);
        //处理事件
        for(int i = 0;i < n;i++){
            int fd = m_epoll.getEventFd(i);
            uint32_t events = m_epoll.getEvents(i);
            if(fd == m_listenFd){
                //处理新连接
                if(events & EPOLLIN){
                    logger.Log(LogLevel::Debug,"Worker %d accept new connection",getpid());
                    acceptConnection();
                }
            }
            else{
                //处理已连接的客户端
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

//处理客户端的读请求
void Worker::handleClientRead(HttpConnection &conn){
    //读取客户端的请求
    //本来想用结构化绑定，但是没办法升级到c++17，所以只能用pair了
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
        //如果是长连接，那么就继续监听读事件
        if(!m_epoll.mod(conn.getFd(),EPOLLIN)){
            Logger::getInstance().Log(LogLevel::Error,"Worker %d mod client fd to epoll failed:%s",getpid(),strerror(errno));
            closeConnection(conn);
            return;
        }
    }
    else{
        //如果是短连接，那么就关闭连接
        closeConnection(conn);
    }
 
}

//处理客户端的写请求
void Worker::handleClientWrite(HttpConnection &conn){
    return;
}

//处理新连接
void Worker::acceptConnection(){
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientFd = accept(m_listenFd,(struct sockaddr*)&clientAddr,&clientAddrLen);
    if(clientFd < 0){
        return;
    }

    Socket clientSocket(clientFd);
    //设置客户端套接字的linger选项
    if(!clientSocket.setLinger()){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d set client fd linger failed:%s",getpid(),strerror(errno));
        close(clientFd);
        return;
    }
    //加入到epoll
    if(!m_epoll.add(clientFd,EPOLLIN)){
        Logger::getInstance().Log(LogLevel::Error,"Worker %d add client fd to epoll failed:%s",getpid(),strerror(errno));
        close(clientFd);
        return;
    }
    else{
        Logger::getInstance().Log(LogLevel::Debug,"Worker %d add client fd to epoll success",getpid());
    }

    //加入到连接池
    m_connections[clientFd].init(clientFd,clientAddr);

}

//关闭连接
void Worker::closeConnection(HttpConnection &conn){
    m_epoll.del(conn.getFd());
    m_connections.erase(conn.getFd());
    conn.close();
}

}//end namespace dohna