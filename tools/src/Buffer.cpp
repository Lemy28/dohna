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

//savedErrno��һ��ָ�룬ָ��һ��int���͵ı��������������������errno,ΪʲôҪ�������أ�
//��Ϊerrno��һ���꣬����ֵ��һ��ȫ�ֱ���������ڶ��̵߳�����£�����̶߳��ڵ���readFd��������ôerrno��ֵ�ͻᱻ����
//�������ʹ��savedErrno���ָ�룬��ôÿ���̶߳����Լ���errno�������Ͳ��ᱻ������
//��Ȼ�����Ƕ���̵ķ�����������Ϊ��ʹ��������ͨ�ã����ǻ���ʹ��savedErrno���ָ��

//������ֻ�ܴ���65536���ֽڵ�����
std::pair<int,bool> Buffer::readFd(int fd) {
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
        return {n,false};
    }
    else if (n <= writable) {
        m_writeIndex += n;
    }
    else {
        m_writeIndex = m_buffer.size();
        append(extrabuf, n - writable);
    }
    return {n,true};
}

//writeFd�����Ĺ����ǽ��������е�����д�뵽�ļ�������fd��
std::pair<int,bool> Buffer::writeFd(int fd){
    ssize_t n = 0;
    while (readableBytes() > 0) {
        int res = write(fd, m_readIndex + m_buffer.data(), readableBytes());
        if (res < 0) {
            return {n,false};
        }
        else{
        n += res;
        m_readIndex += res;
        }
    }
    return {n,true};
}





}   // namespace dohna