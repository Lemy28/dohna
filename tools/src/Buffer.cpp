#include <tools/Buffer.h>
#include <unistd.h>//write

ssize_t Buffer::readFd(int fd, int* savedErrno) {
    //Ϊ�˱����ε���readϵͳ���ã�����ʹ��readvϵͳ����,������cache��˼��
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
        //�����ȡ������С�ڿ�д�����ݣ�˵����ȡ�����ݶ���д�뵽buffer�е�
        m_writeIndex += n;
    }
    else {
        //�����ȡ�����ݴ��ڿ�д�����ݣ�˵����ȡ����������һ����д�뵽��buffer�У���һ����д�뵽��extrabuf��
        m_writeIndex = m_buffer.size();
        append(extrabuf, n - writable);
    }
    return n;
}

ssize_t Buffer::writeFd(int fd, int* savedErrno) {
    std::size_t n = readableBytes();
    ssize_t nwrite = write(fd, peek(), n);
    if (nwrite < 0) {
        //���д��ʧ�ܣ����������
        *savedErrno = errno;
    }
    else {
        //���д��ɹ��������Ѷ�ȡ�����ݵĴ�С
        m_readIndex += nwrite;
    }
    return nwrite;
}