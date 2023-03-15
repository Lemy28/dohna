#include <iostream>
#include <tools/Logger.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <core/WorkerManager.h>

// using namespace dohna;

int main(int argc, char **argv) {
    
    prctl(PR_SET_NAME,"test program");

    WorkerManager::getInstance().run();

    
    std::cout<<"bye bye"<<std::endl;

}