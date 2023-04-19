#include <net/HttpRequest.h>
#include <algorithm>
#include <tools/Logger.h>
#include <regex>

namespace dohna{

const std::string HttpRequest::s_nullstr = "";

//����ͷ�ļ�ʵ�ֵ�HttpRequest��

HttpRequest::HttpRequest(){

    m_method = "";
    m_version = "";
    m_path = "";
    m_body = "";
    
    m_state = ParseState::ExpectRequestLine;

    //��ʼ������ͷ
    m_headers.clear();

}


HttpRequest::~HttpRequest()
{
}

//����������
bool HttpRequest::parseRequestLine(const std::string& line){

    //ʹ��������ʽ��֤������
    //�����и�ʽ�����󷽷� ����·�� HTTP�汾
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;

    if(std::regex_match(line,subMatch,patten)){
        //ΪʲôҪʹ��subMatch[1]��������subMatch[0]��
        //��ΪsubMatch[0]������ƥ����ַ�������subMatch[1]�ǵ�һ����ƥ����ַ���
        m_method = subMatch[1];
        m_path = subMatch[2];
        m_version = subMatch[3];

        //�����󷽷�ת��Ϊ��д
        std::transform(m_method.begin(),m_method.end(),m_method.begin(),::toupper);

        //������汾ת��Ϊ��д
        std::transform(m_version.begin(),m_version.end(),m_version.begin(),::toupper);

        m_state = ParseState::ExpectHeaders;

        return true;
    }
    m_state = ParseState::Unknow;
    // m_code = "400";
    return false;
}

//��������ͷ
void HttpRequest::parseHeaders(const std::string& line){

    //ʹ��������ʽ��֤����ͷ
    //����ͷ��ʽ������ͷ��:����ͷֵ
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;

    if(std::regex_match(line,subMatch,patten)){
        m_headers[subMatch[1]] = subMatch[2];

    }else{
        m_state = ParseState::ExpectBody;
    }

}

//����������
void HttpRequest::parseBody(const std::string& line){
    m_body = line;
    m_state = ParseState::GotAll;
    //��ʱ������������


}

//����http����
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
        //����ָ���ƶ�����һ��
        buffer.retrieveUntil(lineEnd+2);
    }

    Logger::getInstance().Log(LogLevel::Debug,"parseRequest success,method:%s,path:%s,version:%s",m_method.c_str(),m_path.c_str(),m_version.c_str());
    return true;

}

//��ȡ����ͷ
const std::string& HttpRequest::getHeader(const std::string& key) const{
    auto it = m_headers.find(key);
    if(it != m_headers.end()){
        return it->second;
    }
    return s_nullstr;
}


//����uri
void HttpRequest::parsePath(){
    m_path = "/index.html";
    //��ʱ������uri
}

//�ж��Ƿ�Ϊ�����ӣ�����Ҫ��http�汾Ϊ1.1������Ϊhttp1.0Ĭ��Ϊ�����ӣ�http1.1Ĭ��Ϊ������
bool HttpRequest::isKeepAlive() const{
    if(m_headers.find("Connection") != m_headers.end()){
        return m_headers.find("Connection")->second == "keep-alive" && m_version == "HTTP/1.1";
    }
    return false;
}
//��������
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