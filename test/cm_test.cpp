#include <iostream>
#include <tools/Logger.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <core/WorkerManager.h>
#include <tools/ConfigManager.h>
#include <fstream>

// using namespace dohna;

int main(int argc, char **argv) {
    


    ConfigManager& cm = ConfigManager::getInstance();
    cm.loadConfig("/etc/dohna.conf");
    std::cout<<"workercount:"<<cm.getInt("workerProcesses")<<std::endl;
    std::cout<<"if daemon:"<<cm.getString("daemon")<<std::endl;



    sleep(999);

}