#include <tools/HeapTimer.h>

namespace dohna{

//添加定时器
//id:定时器id timeout:定时器超时时间 cb:定时器回调函数
//id是用来标识定时器的，可以是任意值，比如可以是连接的fd，也可以是连接的指针
void HeapTimer::addTimer(int id, int timeout, const TimerCallback &cb)
{
    size_t i;
    //如果定时器已经存在，那么先删除
    //为什么要先删除呢，因为如果定时器已经存在，那么就要更新定时器的超时时间
    if (m_ref.count(id))
    {
        //如果定时器已经存在，那么先删除

        //根据id找到定时器在数组中的下标
        i = m_ref[id];
        m_heap[i].cb = cb;
        m_heap[i].expire = Clock::now() + Ms(timeout);

        //为什么调整堆需要先向下调整，再向上调整呢
        //因为向下调整的时候，可能会破坏堆的性质，所以需要向上调整
        percolateDown(i);
        percolateUp(i);
    }
    else
    {
        //如果定时器不存在，那么插入一个新的定时器

        i = m_heap.size();
        //m_ref.insert({id, i});可以吗？答案是可以的，但是效率不如下面的写法
        m_ref[id] = i;
        m_heap.push_back({id, Clock::now() + Ms(timeout), cb});
        percolateUp(i);
    }
}

//删除定时器
void HeapTimer::delTimer(int id)
{
    if (!m_ref.count(id))
        return;
    size_t i = m_ref[id];
    //为什么要先删除呢，因为删除定时器的时候，需要调整堆
    heapDelete(i);
}

//最小堆的向下调整算法,percolate means 渗透,hole means 洞,在这里的意思是，从洞里渗透出来
void HeapTimer::percolateDown(size_t hole)
{
    //为什么要用temp呢，因为如果直接用m_heap[hole]，那么在下面的循环中，就会把m_heap[hole]的值改变
    HeapTimerNode temp = m_heap[hole];
    //为什么要用size_t呢，因为如果用int，那么当hole是一个很大的数的时候，就会溢出
    size_t child = 0;
    //为什么要用m_heap.size() - 1呢，因为堆的下标是从1开始的，而不是从0开始的
    for (; ((hole << 1) <= m_heap.size() - 1); hole = child)
    {   //<<操作符是左移操作符，左移一位相当于乘以2,为什么要乘以2呢，因为堆是完全二叉树，所以左孩子的下标是2i，右孩子的下标是2i+1
        //为什么不直接写为child = hole * 2;呢，因为这样写的话，如果hole是一个很大的数，那么就会溢出
        //这样写的话，如果hole是一个很大的数，溢出的话，那么就会变成一个很小的数，这样就会造成错误
        
        child = hole << 1;
        if ((child != m_heap.size() - 1) && (m_heap[child + 1] < m_heap[child]))
        //如果右孩子存在，并且右孩子的值小于左孩子的值，那么child就等于右孩子的下标
            ++child;
        if (m_heap[child] < temp)
        //如果孩子的值小于temp的值，那么就把孩子的值赋给m_heap[hole]
            m_heap[hole] = m_heap[child];
        else
            break;
    }
    m_heap[hole] = temp;

}
//最小堆的向上调整算法
void HeapTimer::percolateUp(size_t hole)
{
    HeapTimerNode temp = m_heap[hole];
    for (; hole > 1 && temp < m_heap[hole >> 1]; hole >>= 1)
        m_heap[hole] = m_heap[hole >> 1];
    m_heap[hole] = temp;
}

//最小堆的删除算法，根据传入的下标，删除堆中的定时器
void HeapTimer::heapDelete(size_t hole)
{
    m_heap[hole] = m_heap.back();
    m_ref[m_heap[hole].id] = hole;
    m_heap.pop_back();
    m_ref.erase(m_ref.find(m_heap[hole].id));
    if (hole == m_heap.size())
        return;
    percolateDown(hole);
    percolateUp(hole);
}
//得到堆顶的定时器
HeapTimerNode HeapTimer::top() const
{
    return m_heap[1];
}

//删除堆顶的定时器
void HeapTimer::pop()
{
    heapDelete(1);
}

//心搏函数
void HeapTimer::tick(){
    if(empty())
        return;
    while(!empty()){
        HeapTimerNode node = top();
        //如果堆顶的定时器还没有到期，那么就直接返回
        if(std::chrono::duration_cast<Ms>(node.expire - Clock::now()).count() > 0)
            break;
        //否则就执行堆顶的定时器的回调函数
        node.cb();
        pop();
    }
}


//得到堆的大小
size_t HeapTimer::size() const
{
    return m_heap.size() - 1;
}
//判断堆是否为空
bool HeapTimer::empty() const
{
    return m_heap.size() == 1;
}

//清空堆
void HeapTimer::clear()
{
    m_heap.clear();
    m_ref.clear();
}

//调整定时器，主要是调整定时器的超时时间
void HeapTimer::adjust(int id, int timeout)
{
    if (!m_ref.count(id))
        return;
    size_t i = m_ref[id];
    m_heap[i].expire = Clock::now() + Ms(timeout);
    percolateDown(i);
    percolateUp(i);


}

//得到堆顶的定时器的超时时间
TimeStamp HeapTimer::topExpire() const
{
    return m_heap[1].expire;
}

//得到堆顶的定时器的id
int HeapTimer::topId() const
{
    return m_heap[1].id;
}

//得到下一个定时器的超时时间
int HeapTimer::getNextTick()
{   
    tick();
    size_t res = -1;
    if(!empty())
        res = std::chrono::duration_cast<Ms>(topExpire() - Clock::now()).count();
    if(res < 0)
        res = 0;
    return res;

}

}
