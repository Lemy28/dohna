#include<net/HttpConnection.h>
#include<tools/Logger.h>
namespace dohna{

HttpConnection::HttpConnection(int clientFd,sockaddr_in clientAddr)
    :m_socket(clientFd),m_clientAddr(clientAddr)
{
}

HttpConnection::~HttpConnection()
{}

void HttpConnection::init(int clientFd,sockaddr_in clientAddr)
{
    m_socket = clientFd;
    m_clientAddr = clientAddr;
    m_isClosed = false;
}



std::pair<int,bool> HttpConnection::recv()
{
    return m_inputBuffer.readFd(m_socket.getFd());
  
}

std::pair<int,bool> HttpConnection::send()
{
    return m_outputBuffer.writeFd(m_socket.getFd());
}

//¥¶¿Ìhttp«Î«Û
bool HttpConnection::handle()
{
    // m_request.parseRequest(m_inputBuffer);
    m_response.handleRequest(m_request);
    m_response.makeResponse(m_outputBuffer);
    send();

    return true;

}



}