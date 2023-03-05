#include <tools/config.h>
#include <iostream>
#include <tools/ProcessTitle.h>
#include <unistd.h>

using namespace dohna;

int main(int argc, char **argv) {
    std::cout<<"environ[0]:"<<environ[0]<<std::endl;
    std::cout<<"environ[1]:"<<environ[1]<<std::endl;
    ProcessTitle pt(argc, argv, environ);
    pt.setTitle("dohna123456789");

    std::cout<<"environ[0]:"<<environ[0]<<std::endl;
    std::cout<<"environ[1]:"<<environ[1]<<std::endl;
    std::cout<<"environ[2]:"<<environ[2]<<std::endl;

    while (true) {}
    return -1;
}