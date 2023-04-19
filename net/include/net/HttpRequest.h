#pragma once
#include <string>
#include <unordered_map>
#include <tools/Buffer.h>
namespace dohna{


//http������,���ڽ���http����
//��������״̬������http����
//ͬʱʹ��������ʽ��֤http����
class HttpRequest
{   

public:
    enum class ParseState{
        Unknow,
        ExpectRequestLine,
        ExpectHeaders,
        ExpectBody,
        GotAll
    };

    enum class HttpMethod{
        Unknow,
        GET,
        POST,
        HEAD,
        PUT,
        DELETE
    };
    enum class HttpCode{
        Unknow,
        Ok = 200,
        BadRequest = 400,
        NotFound = 404
    };


    //����http����
    bool parseRequest(Buffer& buffer);

    HttpRequest();
    ~HttpRequest();

    ParseState getState() const {return m_state;}
    const std::string& getPath() const {return m_path;}
    const std::string& getBody() const {return m_body;}
    const std::unordered_map<std::string,std::string>& getHeaders() const {return m_headers;}
    const std::string& getHeader(const std::string& key) const;
    const std::string& getMethod() const {return m_method;}
    const std::string& getVersion() const {return m_version;}
    const std::string& getContentType() const;
    const std::string& getContentLength() const;
    const std::string& getServer() const;
    const std::string& getDate() const;
    const std::string& getLastModified() const;
    const std::string& getHost() const;
    const std::string& getAccept() const;
    const std::string& getAcceptLanguage() const;
    const std::string& getAcceptEncoding() const;
    const std::string& getReferer() const;
    const std::string& getConnection() const;
    const std::string& getUpgradeInsecureRequests() const;
    const std::string& getUserAgent() const;

    void reset();
    bool isKeepAlive() const;
    static const std::string s_nullstr;

private:
    ParseState m_state;
    std::string m_method;
    std::string m_path;
    std::string m_version;
    std::string m_body;
    std::unordered_map<std::string,std::string> m_headers;

    //����������
    bool parseRequestLine(const std::string& line);
    //��������ͷ
    void parseHeaders(const std::string& line);
    //����������
    void parseBody(const std::string& line);

    //����uri
    void parsePath();

    void badRequest();



    // //����http���󷽷�
    // void setMethod(const std::string& method);
    // //����http����·��
    // void setPath(const std::string& path);
    // //����http����汾
    // void setVersion(const std::string& version);
    // //����http����ͷ
    // void setHeader(const std::string& key, const std::string& value);
    // //����http������
    // void setBody(const std::string& body);
    // //����http����״̬
    // void setState(ParseState state);
    // //����http����״̬��
    // void setStatusCode(HttpStatusCode code);
    // //����http����״̬��Ϣ
    // void setStatusMessage(const std::string& message);
    // //����http����״̬
    // void setKeepAlive(bool on);
    // //����http����״̬
    // void setCloseConnection(bool on);





};



}