#pragma once

#include <tools/ConfigManager.h>
#include <unistd.h>
#include <tools/Logger.h>

class ChildProcess {
public:
    ChildProcess() {}
    virtual ~ChildProcess() {}

    void run() {
        // �ӽ��̾���ִ�е��߼�

        Logger &logger = Logger::getInstance();
        logger.Log(LogLevel::Info,"Child process running with pid: %d \n",getpid());
        sleep(999999);
        // �ӽ���ִ����Ϻ�ͨ���˳���֪ͨ������
        exit(0);
    }
};


class WorkerManager
{

private:
    int workerProcesses_;

public:

    static WorkerManager& getInstance();
    WorkerManager(const WorkerManager&) = delete;
    WorkerManager& operator=(const WorkerManager&) = delete;
    void run();

private:
    void createWorker();

    WorkerManager();
    ~WorkerManager();
};

