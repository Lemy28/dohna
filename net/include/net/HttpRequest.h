#pragma once
#include <string>
#include <unordered_map>
#include <tools/Buffer.h>
namespace dohna{


//http请求类,用于解析http请求
//采用有限状态机解析http请求
//同时使用正则表达式验证http请求
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


    //解析http请求
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

    //解析请求行
    bool parseRequestLine(const std::string& line);
    //解析请求头
    void parseHeaders(const std::string& line);
    //解析请求体
    void parseBody(const std::string& line);

    //解析uri
    void parsePath();

    void badRequest();



    // //设置http请求方法
    // void setMethod(const std::string& method);
    // //设置http请求路径
    // void setPath(const std::string& path);
    // //设置http请求版本
    // void setVersion(const std::string& version);
    // //设置http请求头
    // void setHeader(const std::string& key, const std::string& value);
    // //设置http请求体
    // void setBody(const std::string& body);
    // //设置http请求状态
    // void setState(ParseState state);
    // //设置http请求状态码
    // void setStatusCode(HttpStatusCode code);
    // //设置http请求状态信息
    // void setStatusMessage(const std::string& message);
    // //设置http请求状态
    // void setKeepAlive(bool on);
    // //设置http请求状态
    // void setCloseConnection(bool on);





};



}