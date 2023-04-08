#pragma once

#include <tools/ConfigManager.h>
#include <unistd.h>
#include <tools/Logger.h>

namespace dohna
{
class WorkerManager
{

private:
    int m_workerProcesses;

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


}
