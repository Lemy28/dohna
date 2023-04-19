#include <tools/HeapTimer.h>

namespace dohna{

//��Ӷ�ʱ��
//id:��ʱ��id timeout:��ʱ����ʱʱ�� cb:��ʱ���ص�����
//id��������ʶ��ʱ���ģ�����������ֵ��������������ӵ�fd��Ҳ���������ӵ�ָ��
void HeapTimer::addTimer(int id, int timeout, const TimerCallback &cb)
{
    size_t i;
    //�����ʱ���Ѿ����ڣ���ô��ɾ��
    //ΪʲôҪ��ɾ���أ���Ϊ�����ʱ���Ѿ����ڣ���ô��Ҫ���¶�ʱ���ĳ�ʱʱ��
    if (m_ref.count(id))
    {
        //�����ʱ���Ѿ����ڣ���ô��ɾ��

        //����id�ҵ���ʱ���������е��±�
        i = m_ref[id];
        m_heap[i].cb = cb;
        m_heap[i].expire = Clock::now() + Ms(timeout);

        //Ϊʲô��������Ҫ�����µ����������ϵ�����
        //��Ϊ���µ�����ʱ�򣬿��ܻ��ƻ��ѵ����ʣ�������Ҫ���ϵ���
        percolateDown(i);
        percolateUp(i);
    }
    else
    {
        //�����ʱ�������ڣ���ô����һ���µĶ�ʱ��

        i = m_heap.size();
        //m_ref.insert({id, i});�����𣿴��ǿ��Եģ�����Ч�ʲ��������д��
        m_ref[id] = i;
        m_heap.push_back({id, Clock::now() + Ms(timeout), cb});
        percolateUp(i);
    }
}

//ɾ����ʱ��
void HeapTimer::delTimer(int id)
{
    if (!m_ref.count(id))
        return;
    size_t i = m_ref[id];
    //ΪʲôҪ��ɾ���أ���Ϊɾ����ʱ����ʱ����Ҫ������
    heapDelete(i);
}

//��С�ѵ����µ����㷨,percolate means ��͸,hole means ��,���������˼�ǣ��Ӷ�����͸����
void HeapTimer::percolateDown(size_t hole)
{
    //ΪʲôҪ��temp�أ���Ϊ���ֱ����m_heap[hole]����ô�������ѭ���У��ͻ��m_heap[hole]��ֵ�ı�
    HeapTimerNode temp = m_heap[hole];
    //ΪʲôҪ��size_t�أ���Ϊ�����int����ô��hole��һ���ܴ������ʱ�򣬾ͻ����
    size_t child = 0;
    //ΪʲôҪ��m_heap.size() - 1�أ���Ϊ�ѵ��±��Ǵ�1��ʼ�ģ������Ǵ�0��ʼ��
    for (; ((hole << 1) <= m_heap.size() - 1); hole = child)
    {   //<<�����������Ʋ�����������һλ�൱�ڳ���2,ΪʲôҪ����2�أ���Ϊ������ȫ���������������ӵ��±���2i���Һ��ӵ��±���2i+1
        //Ϊʲô��ֱ��дΪchild = hole * 2;�أ���Ϊ����д�Ļ������hole��һ���ܴ��������ô�ͻ����
        //����д�Ļ������hole��һ���ܴ����������Ļ�����ô�ͻ���һ����С�����������ͻ���ɴ���
        
        child = hole << 1;
        if ((child != m_heap.size() - 1) && (m_heap[child + 1] < m_heap[child]))
        //����Һ��Ӵ��ڣ������Һ��ӵ�ֵС�����ӵ�ֵ����ôchild�͵����Һ��ӵ��±�
            ++child;
        if (m_heap[child] < temp)
        //������ӵ�ֵС��temp��ֵ����ô�ͰѺ��ӵ�ֵ����m_heap[hole]
            m_heap[hole] = m_heap[child];
        else
            break;
    }
    m_heap[hole] = temp;

}
//��С�ѵ����ϵ����㷨
void HeapTimer::percolateUp(size_t hole)
{
    HeapTimerNode temp = m_heap[hole];
    for (; hole > 1 && temp < m_heap[hole >> 1]; hole >>= 1)
        m_heap[hole] = m_heap[hole >> 1];
    m_heap[hole] = temp;
}

//��С�ѵ�ɾ���㷨�����ݴ�����±꣬ɾ�����еĶ�ʱ��
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
//�õ��Ѷ��Ķ�ʱ��
HeapTimerNode HeapTimer::top() const
{
    return m_heap[1];
}

//ɾ���Ѷ��Ķ�ʱ��
void HeapTimer::pop()
{
    heapDelete(1);
}

//�Ĳ�����
void HeapTimer::tick(){
    if(empty())
        return;
    while(!empty()){
        HeapTimerNode node = top();
        //����Ѷ��Ķ�ʱ����û�е��ڣ���ô��ֱ�ӷ���
        if(std::chrono::duration_cast<Ms>(node.expire - Clock::now()).count() > 0)
            break;
        //�����ִ�жѶ��Ķ�ʱ���Ļص�����
        node.cb();
        pop();
    }
}


//�õ��ѵĴ�С
size_t HeapTimer::size() const
{
    return m_heap.size() - 1;
}
//�ж϶��Ƿ�Ϊ��
bool HeapTimer::empty() const
{
    return m_heap.size() == 1;
}

//��ն�
void HeapTimer::clear()
{
    m_heap.clear();
    m_ref.clear();
}

//������ʱ������Ҫ�ǵ�����ʱ���ĳ�ʱʱ��
void HeapTimer::adjust(int id, int timeout)
{
    if (!m_ref.count(id))
        return;
    size_t i = m_ref[id];
    m_heap[i].expire = Clock::now() + Ms(timeout);
    percolateDown(i);
    percolateUp(i);


}

//�õ��Ѷ��Ķ�ʱ���ĳ�ʱʱ��
TimeStamp HeapTimer::topExpire() const
{
    return m_heap[1].expire;
}

//�õ��Ѷ��Ķ�ʱ����id
int HeapTimer::topId() const
{
    return m_heap[1].id;
}

//�õ���һ����ʱ���ĳ�ʱʱ��
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
