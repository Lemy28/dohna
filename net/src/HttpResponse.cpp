#include <net/HttpResponse.h>
#include <chrono>

namespace dohna{


HttpResponse::HttpResponse(){
}

HttpResponse::~HttpResponse()
{
}

//添加响应行
void HttpResponse::addResponseLine(Buffer& buffer)
{
    buffer.append("HTTP/1.1 ");
    buffer.append(std::to_string(static_cast<int>(m_statusCode)));
    buffer.append(" ");
    buffer.append(m_statusMessage);
    buffer.append("\r\n");
}

//添加响应头
void HttpResponse::addHeader(Buffer& buffer)
{
    buffer.append("Server: ");
    buffer.append(m_server);
    buffer.append("\r\n");

    buffer.append("Date: ");
    m_date = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    buffer.append(m_date);
    buffer.append("\r\n");

    buffer.append("Content-Type: ");
    buffer.append(m_contentType);
    buffer.append("\r\n");

    buffer.append("Content-Length: ");
    buffer.append(m_contentLength);
    buffer.append("\r\n");

    buffer.append("Last-Modified: ");
    buffer.append(m_lastModified);
    buffer.append("\r\n");

    buffer.append("Connection: ");
    if (m_isKeepAlive)
    {
        buffer.append("keep-alive");
    }
    else
    {
        buffer.append("close");
    }
    buffer.append("\r\n");

    buffer.append("\r\n");
}
//添加响应体
void HttpResponse::addBody(Buffer& buffer)
{
    buffer.append(m_body);
}

//处理http请求
void HttpResponse::handleRequest(HttpRequest& request)
{   
    m_body = request.getBody();
    m_isKeepAlive = request.isKeepAlive();
    m_version = request.getVersion();
    m_statusCode = HttpStatusCode::k200Ok;
    m_statusMessage = "OK";
    m_contentType = "text/html";
    m_contentLength = std::to_string(m_body.size());
    m_server = "dohna";
    m_lastModified = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

}

//构造http响应
void HttpResponse::makeResponse(Buffer& buffer)
{
    addResponseLine(buffer);
    addHeader(buffer);
    addBody(buffer);
}


}