#pragma once

#include <tools/ConfigManager.h>
#include <unistd.h>
#include <tools/Logger.h>

class ChildProcess {
public:
    ChildProcess() {}
    virtual ~ChildProcess() {}

    void run() {
        // 子进程具体执行的逻辑

        Logger &logger = Logger::getInstance();
        logger.Log(LogLevel::Info,"Child process running with pid: %d \n",getpid());
        sleep(999999);
        // 子进程执行完毕后，通过退出码通知父进程
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

