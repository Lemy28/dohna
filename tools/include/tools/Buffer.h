#pragma once

//封装一个buffer类

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

    //获取回车换行符的位置
    const char* findCRLF()const;

    //获取可读数据的大小
    std::size_t readableBytes() const { return m_writeIndex - m_readIndex; }
    //获取可写数据的大小
    std::size_t writableBytes() const { return m_buffer.size() - m_writeIndex; }
    //获取已经读取的数据的大小
    std::size_t prependableBytes() const { return m_readIndex; }

    //获取可读数据的首地址
    const char* peek() const { return begin() + m_readIndex; }
    //获取可写数据的首地址
    char* beginWrite() { return begin() + m_writeIndex; }
    //const char* beginWrite() const { return begin() + m_writeIndex; }
    //更新可写数据的大小
    void hasWritten(std::size_t len) { m_writeIndex += len; }
    //从buffer中读取指定长度的数据
    void retrieve(std::size_t len) { m_readIndex += len; }
    //从buffer中读取指定位置的数据
    void retrieveUntil(const char* end) { retrieve(end - peek()); }
    //清空buffer中的数据
    void retrieveAll() { m_readIndex = 0; m_writeIndex = 0; }
    //清空buffer中的数据并转换为字符串
    std::string retrieveAllAsString() { return retrieveAsString(readableBytes()); }
    //从buffer中读取指定长度的数据并转换为字符串
    std::string retrieveAsString(std::size_t len) {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    //在buffer中写入数据
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

    //从fd中读取数据
    std::pair<int,bool> readFd(int fd);
    //向fd中写入数据
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