#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace dohna{


using TimerCallback = std::function<void()>;
using Clock = std::chrono::system_clock;
using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;
using Ms = std::chrono::milliseconds;

//定时器节点
struct HeapTimerNode
{
    int id;
    TimeStamp expire;
    TimerCallback cb;
    bool operator<(const HeapTimerNode &rhs) const
    {
        return expire < rhs.expire;
    }

    bool isValid() const
    {
        return cb != nullptr;
    }
};

//实现一个小根堆，用于定时器

//定时器在web服务器中，有什么用呢
//可以用来实现超时断开连接，比如客户端连接服务器后，一段时间没有发送数据，就断开连接
//可以用来实现定时任务，比如每隔一段时间，执行某个任务

class HeapTimer
{
    //定时器数组,用于存放定时器
    //为什么不用priority_queue，因为priority_queue没有提供删除元素的接口,但是我们需要删除元素
    std::vector<HeapTimerNode> m_heap;
    //定时器id和数组下标的映射,用于删除定时器
    //为什么要用unordered_map，因为删除定时器的时候，需要根据id找到数组下标，然后删除
    std::unordered_map<int, size_t> m_ref;

    //最小堆的向下调整算法
    void percolateDown(size_t hole);
    //最小堆的向上调整算法
    void percolateUp(size_t hole);
    //最小堆的删除算法
    void heapDelete(size_t hole);
    //最小堆的插入算法
    void heapInsert(size_t hole, HeapTimerNode node);

public:
    HeapTimer() : m_heap(1) {}
    ~HeapTimer() {clear();}

    //添加定时器
    void addTimer(int id, int timeout, const TimerCallback &cb);
    //删除定时器
    void delTimer(int id);
    //判断堆是否为空
    bool empty() const;
    //心搏函数
    void tick();
    //调整定时器的时间
    void adjust(int id, int timeout);
    //删除所有定时器
    void clear();
    //获取堆中定时器的个数
    size_t size() const;
    //删除堆顶定时器
    void pop();
    //得到堆顶定时器
    HeapTimerNode top()const ;

    //得到堆顶定时器的超时时间
    TimeStamp topExpire() const;
    //得到堆顶定时器的id
    int topId() const;

    //获取下一个超时时间
    int getNextTick();



};




}