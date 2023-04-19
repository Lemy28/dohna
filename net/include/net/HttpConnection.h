#pragma once

#include <tools/Buffer.h>
#include <net/Socket.h>
#include <net/HttpRequest.h>
#include <net/HttpResponse.h>

namespace dohna{


//在Socket类的基础上封装一个HttpConnection类

class HttpConnection
{

public:
    HttpConnection(int clientFd,sockaddr_in clientAddr);
    HttpConnection() = default;
    ~HttpConnection();

    void init(int clientFd,sockaddr_in clientAddr);
    //获取socket的fd
    int getFd() { return m_socket.getFd(); }
    //关闭socket
    void close() { m_socket.close(); m_isClosed = true; }
    //判断socket是否关闭
    bool isClosed() { return m_isClosed; }
    //判断是否是长连接
    bool isKeepAlive() { return m_request.isKeepAlive(); }

    //读取数据
    std::pair<int,bool> recv();
    //写入数据
    std::pair<int,bool> send();
    //处理http请求
    bool handle();
    
    

private:


    sockaddr_in m_clientAddr;
    Socket m_socket;
    bool m_isClosed = false;

    Buffer m_inputBuffer;
    Buffer m_outputBuffer;

    HttpRequest m_request;
    HttpResponse m_response;



};





}