#include <net/HttpRequest.h>
#include <algorithm>
#include <tools/Logger.h>
#include <regex>

namespace dohna{

const std::string HttpRequest::s_nullstr = "";

//根据头文件实现的HttpRequest类

HttpRequest::HttpRequest(){

    m_method = "";
    m_version = "";
    m_path = "";
    m_body = "";
    
    m_state = ParseState::ExpectRequestLine;

    //初始化请求头
    m_headers.clear();

}


HttpRequest::~HttpRequest()
{
}

//解析请求行
bool HttpRequest::parseRequestLine(const std::string& line){

    //使用正则表达式验证请求行
    //请求行格式：请求方法 请求路径 HTTP版本
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;

    if(std::regex_match(line,subMatch,patten)){
        //为什么要使用subMatch[1]，而不是subMatch[0]？
        //因为subMatch[0]是整个匹配的字符串，而subMatch[1]是第一个子匹配的字符串
        m_method = subMatch[1];
        m_path = subMatch[2];
        m_version = subMatch[3];

        //将请求方法转换为大写
        std::transform(m_method.begin(),m_method.end(),m_method.begin(),::toupper);

        //将请求版本转换为大写
        std::transform(m_version.begin(),m_version.end(),m_version.begin(),::toupper);

        m_state = ParseState::ExpectHeaders;

        return true;
    }
    m_state = ParseState::Unknow;
    // m_code = "400";
    return false;
}

//解析请求头
void HttpRequest::parseHeaders(const std::string& line){

    //使用正则表达式验证请求头
    //请求头格式：请求头名:请求头值
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;

    if(std::regex_match(line,subMatch,patten)){
        m_headers[subMatch[1]] = subMatch[2];

    }else{
        m_state = ParseState::ExpectBody;
    }

}

//解析请求体
void HttpRequest::parseBody(const std::string& line){
    m_body = line;
    m_state = ParseState::GotAll;
    //暂时不处理请求体


}

//解析http请求
bool HttpRequest::parseRequest(Buffer& buffer){

    if(buffer.readableBytes() <= 0){
        return false;
    }
    while(buffer.readableBytes() && m_state != ParseState::GotAll){
        const char* lineEnd = buffer.findCRLF();
        std::string line(buffer.peek(),lineEnd);

        switch (m_state)
        {
        case ParseState::ExpectRequestLine:
            parseRequestLine(line);
            break;
        case ParseState::ExpectHeaders:
            parseHeaders(line);
            break;
        case ParseState::ExpectBody:
            parseBody(line);
            break;
        case ParseState::Unknow:
            badRequest();
            break;
        default:
            break;
        }

        if(lineEnd == buffer.beginWrite()){
            break;
        }
        //将读指针移动到下一行
        buffer.retrieveUntil(lineEnd+2);
    }

    Logger::getInstance().Log(LogLevel::Debug,"parseRequest success,method:%s,path:%s,version:%s",m_method.c_str(),m_path.c_str(),m_version.c_str());
    return true;

}

//获取请求头
const std::string& HttpRequest::getHeader(const std::string& key) const{
    auto it = m_headers.find(key);
    if(it != m_headers.end()){
        return it->second;
    }
    return s_nullstr;
}


//解析uri
void HttpRequest::parsePath(){
    m_path = "/index.html";
    //暂时不处理uri
}

//判断是否为长连接，这里要求http版本为1.1，是因为http1.0默认为短连接，http1.1默认为长连接
bool HttpRequest::isKeepAlive() const{
    if(m_headers.find("Connection") != m_headers.end()){
        return m_headers.find("Connection")->second == "keep-alive" && m_version == "HTTP/1.1";
    }
    return false;
}
//重置请求
void HttpRequest::reset(){
    m_method = "";
    m_version = "";
    m_path = "";
    m_body = "";
    m_state = ParseState::ExpectRequestLine;
    m_headers.clear();

}

void HttpRequest::badRequest(){
    m_state = ParseState::GotAll;
}



}//namespace dohna