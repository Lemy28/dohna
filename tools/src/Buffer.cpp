#include <tools/Buffer.h>
#include <unistd.h>//write

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