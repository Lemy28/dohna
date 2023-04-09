#include <tools/Buffer.h>
#include <unistd.h>//write
#include <algorithm>//std::search


namespace dohna{

const char Buffer::kCRLF[] = "\r\n";

const char* Buffer::findCRLF() const {
    //ʹ��std::search�������Ѿ�д��������в��һس����з�
    //search�ĵ�һ��������Ҫ���ҵ����ݵ��׵�ַ���ڶ���������Ҫ���ҵ����ݵ�β��ַ
    //������������Ҫ���ҵ����ݵ��׵�ַ�����ĸ�������Ҫ���ҵ����ݵ�β��ַ
    //���������m_readIndex + m_buffer.data()��Ҫ���ҵ����ݵ��׵�ַ��m_writeIndex + m_buffer.data()��Ҫ���ҵ����ݵ�β��ַ
    //kCRLF��Ҫ���ҵ����ݵ��׵�ַ��kCRLF + 2��Ҫ���ҵ����ݵ�β��ַ
    const char* crlf = std::search(m_readIndex + m_buffer.data(), m_writeIndex + m_buffer.data(), kCRLF, kCRLF + 2);
    //Ϊʲô�ڶ���������m_writeIndex + m_buffer.data()��������m_buffer.data() + m_buffer.size()�أ�
    //��Ϊm_buffer.data() + m_buffer.size()��buffer��β��ַ����m_writeIndex + m_buffer.data()�ǿ�д���ݵ�β��ַ
    //����ڶ���������m_buffer.data() + m_buffer.size()����ô�ͻ��������buffer�������ǲ����Ѿ�д�������
    return crlf == m_writeIndex + m_buffer.data() ? nullptr : crlf;
}


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





}   // namespace dohna