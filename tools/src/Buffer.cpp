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

//savedErrno是一个指针，指向一个int类型的变量，这个变量用来保存errno,为什么要这样做呢？
//因为errno是一个宏，它的值是一个全局变量，如果在多线程的情况下，多个线程都在调用readFd函数，那么errno的值就会被覆盖
//但是如果使用savedErrno这个指针，那么每个线程都有自己的errno变量，就不会被覆盖了
//虽然我们是多进程的服务器，但是为了使这个类更加通用，我们还是使用savedErrno这个指针

//缓冲区只能处理65536个字节的数据
std::pair<int,bool> Buffer::readFd(int fd) {
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

//writeFd函数的功能是将缓冲区中的数据写入到文件描述符fd中
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