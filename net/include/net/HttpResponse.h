#pragma once

#include <tools/Buffer.h>
#include <net/HttpRequest.h>

namespace dohna{


//http响应类
//用于构造http响应
//
class HttpResponse
{
private:
//响应状态码
    enum class HttpStatusCode
    {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
    };

public:

    HttpResponse();
    ~HttpResponse();

    //处理http请求
    void handleRequest(HttpRequest& request);

    //构造http响应
    void makeResponse(Buffer& buffer);

    
private:

    //添加响应行
    void addResponseLine(Buffer& buffer);
    //添加响应头
    void addHeader(Buffer& buffer);
    //添加响应体
    void addBody(Buffer& buffer);
    bool m_isKeepAlive;
    HttpStatusCode m_statusCode;
    std::string m_statusMessage;
    std::string m_version;
    std::string m_body;
    std::string m_contentType;
    std::string m_contentLength;
    std::string m_server;
    std::string m_date;
    std::string m_lastModified;

};







}