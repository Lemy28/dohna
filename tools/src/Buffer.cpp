#include <tools/Buffer.h>
#include <unistd.h>//write
#include <algorithm>//std::search


namespace dohna{

const char Buffer::kCRLF[] = "\r\n";

const char* Buffer::findCRLF() const {
    //使用std::search函数在已经写入的数据中查找回车换行符
    //search的第一个参数是要查找的数据的首地址，第二个参数是要查找的数据的尾地址
    //第三个参数是要查找的数据的首地址，第四个参数是要查找的数据的尾地址
    //所以这里的m_readIndex + m_buffer.data()是要查找的数据的首地址，m_writeIndex + m_buffer.data()是要查找的数据的尾地址
    //kCRLF是要查找的数据的首地址，kCRLF + 2是要查找的数据的尾地址
    const char* crlf = std::search(m_readIndex + m_buffer.data(), m_writeIndex + m_buffer.data(), kCRLF, kCRLF + 2);
    //为什么第二个参数是m_writeIndex + m_buffer.data()，而不是m_buffer.data() + m_buffer.size()呢？
    //因为m_buffer.data() + m_buffer.size()是buffer的尾地址，而m_writeIndex + m_buffer.data()是可写数据的尾地址
    //如果第二个参数是m_buffer.data() + m_buffer.size()，那么就会查找整个buffer，而不是查找已经写入的数据
    return crlf == m_writeIndex + m_buffer.data() ? nullptr : crlf;
}


ssize_t Buffer::readFd(int fd, int* savedErrno) {
    //为了避免多次调用read系统调用，我们使用readv系统调用,利用了cache的思想
    char extrabuf[65536];
    struct iovec vec[2];
    const std::size_t writable = writableBytes();
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    const std::size_t n = readv(fd, vec, 2);
    if (n < 0) {
        *savedErrno = errno;
    }
    else if (static_cast<std::size_t>(n) <= writable) {
        //如果读取的数据小于可写的数据，说明读取的数据都是写入到buffer中的
        m_writeIndex += n;
    }
    else {
        //如果读取的数据大于可写的数据，说明读取的数据中有一部分写入到了buffer中，有一部分写入到了extrabuf中
        m_writeIndex = m_buffer.size();
        append(extrabuf, n - writable);
    }
    return n;
}

ssize_t Buffer::writeFd(int fd, int* savedErrno) {
    std::size_t n = readableBytes();
    ssize_t nwrite = write(fd, peek(), n);
    if (nwrite < 0) {
        //如果写入失败，保存错误码
        *savedErrno = errno;
    }
    else {
        //如果写入成功，更新已读取的数据的大小
        m_readIndex += nwrite;
    }
    return nwrite;
}





}   // namespace dohna