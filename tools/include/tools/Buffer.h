#pragma once

//��װһ��buffer��

#include <vector>
#include <atomic>
#include <string>
#include <cstring>//strlen
#include <sys/uio.h>//ssize_t, readv, writev

namespace dohna{



class Buffer{
public:
    Buffer(int size = 1024) : m_buffer(size), m_readIndex(0), m_writeIndex(0) {}
    ~Buffer() = default;

    //��ȡ�س����з���λ��
    const char* findCRLF()const;

    //��ȡ�ɶ����ݵĴ�С
    std::size_t readableBytes() const { return m_writeIndex - m_readIndex; }
    //��ȡ��д���ݵĴ�С
    std::size_t writableBytes() const { return m_buffer.size() - m_writeIndex; }
    //��ȡ�Ѿ���ȡ�����ݵĴ�С
    std::size_t prependableBytes() const { return m_readIndex; }

    //��ȡ�ɶ����ݵ��׵�ַ
    const char* peek() const { return begin() + m_readIndex; }
    //��ȡ��д���ݵ��׵�ַ
    char* beginWrite() { return begin() + m_writeIndex; }
    //const char* beginWrite() const { return begin() + m_writeIndex; }
    //���¿�д���ݵĴ�С
    void hasWritten(std::size_t len) { m_writeIndex += len; }
    //��buffer�ж�ȡָ�����ȵ�����
    void retrieve(std::size_t len) { m_readIndex += len; }
    //��buffer�ж�ȡָ��λ�õ�����
    void retrieveUntil(const char* end) { retrieve(end - peek()); }
    //���buffer�е�����
    void retrieveAll() { m_readIndex = 0; m_writeIndex = 0; }
    //���buffer�е����ݲ�ת��Ϊ�ַ���
    std::string retrieveAllAsString() { return retrieveAsString(readableBytes()); }
    //��buffer�ж�ȡָ�����ȵ����ݲ�ת��Ϊ�ַ���
    std::string retrieveAsString(std::size_t len) {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    //��buffer��д������
    void append(const char* data, std::size_t len) {
        if (writableBytes() < len) {
            m_buffer.resize(m_writeIndex + len);
        }
        std::copy(data, data + len, beginWrite());
        hasWritten(len);
    }
    void append(const std::string& str) { append(str.data(), str.size()); }
    void append(const char* str) { append(str, strlen(str)); }
    void append(const Buffer &buffer) { append(buffer.peek(), buffer.readableBytes()); }

    //��fd�ж�ȡ����
    std::pair<int,bool> readFd(int fd);
    //��fd��д������
    std::pair<int,bool> writeFd(int fd);

private:
    char* begin() { return &*m_buffer.begin(); }
    const char* begin() const { return &*m_buffer.begin(); }

    static const char kCRLF[];

    std::vector<char> m_buffer;
    std::atomic<std::size_t> m_readIndex;
    std::atomic<std::size_t> m_writeIndex;

};



}   // namespace dohna