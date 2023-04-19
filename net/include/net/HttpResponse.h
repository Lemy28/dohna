#pragma once

#include <tools/Buffer.h>
#include <net/HttpRequest.h>

namespace dohna{


//http��Ӧ��
//���ڹ���http��Ӧ
//
class HttpResponse
{
private:
//��Ӧ״̬��
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

    //����http����
    void handleRequest(HttpRequest& request);

    //����http��Ӧ
    void makeResponse(Buffer& buffer);

    
private:

    //�����Ӧ��
    void addResponseLine(Buffer& buffer);
    //�����Ӧͷ
    void addHeader(Buffer& buffer);
    //�����Ӧ��
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