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

//��ʱ���ڵ�
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

//ʵ��һ��С���ѣ����ڶ�ʱ��

//��ʱ����web�������У���ʲô����
//��������ʵ�ֳ�ʱ�Ͽ����ӣ�����ͻ������ӷ�������һ��ʱ��û�з������ݣ��ͶϿ�����
//��������ʵ�ֶ�ʱ���񣬱���ÿ��һ��ʱ�䣬ִ��ĳ������

class HeapTimer
{
    //��ʱ������,���ڴ�Ŷ�ʱ��
    //Ϊʲô����priority_queue����Ϊpriority_queueû���ṩɾ��Ԫ�صĽӿ�,����������Ҫɾ��Ԫ��
    std::vector<HeapTimerNode> m_heap;
    //��ʱ��id�������±��ӳ��,����ɾ����ʱ��
    //ΪʲôҪ��unordered_map����Ϊɾ����ʱ����ʱ����Ҫ����id�ҵ������±꣬Ȼ��ɾ��
    std::unordered_map<int, size_t> m_ref;

    //��С�ѵ����µ����㷨
    void percolateDown(size_t hole);
    //��С�ѵ����ϵ����㷨
    void percolateUp(size_t hole);
    //��С�ѵ�ɾ���㷨
    void heapDelete(size_t hole);
    //��С�ѵĲ����㷨
    void heapInsert(size_t hole, HeapTimerNode node);

public:
    HeapTimer() : m_heap(1) {}
    ~HeapTimer() {clear();}

    //��Ӷ�ʱ��
    void addTimer(int id, int timeout, const TimerCallback &cb);
    //ɾ����ʱ��
    void delTimer(int id);
    //�ж϶��Ƿ�Ϊ��
    bool empty() const;
    //�Ĳ�����
    void tick();
    //������ʱ����ʱ��
    void adjust(int id, int timeout);
    //ɾ�����ж�ʱ��
    void clear();
    //��ȡ���ж�ʱ���ĸ���
    size_t size() const;
    //ɾ���Ѷ���ʱ��
    void pop();
    //�õ��Ѷ���ʱ��
    HeapTimerNode top()const ;

    //�õ��Ѷ���ʱ���ĳ�ʱʱ��
    TimeStamp topExpire() const;
    //�õ��Ѷ���ʱ����id
    int topId() const;

    //��ȡ��һ����ʱʱ��
    int getNextTick();



};




}