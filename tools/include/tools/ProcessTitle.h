#pragma once

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

class ProcessTitle {
public:
    ProcessTitle(int argc, char** argv, char** envp) {
        init(argc, argv, envp);
    }

    void setProcTitle(const char *title); 

private:
    char** m_osArgv;
    char* m_envMem;
    int m_envSize;

    void init(int argc, char** argv, char** envp) {
        if (argc <= 0 || argv == nullptr || envp == nullptr) {
            std::cerr<<"invalid parameter!"<<std::endl;
            exit(1);
        }
        m_osArgv = argv;

        // Calculate environment memory size
        m_envSize = 0;
        for (int i = 0; envp[i]; ++i) {
            m_envSize += strlen(envp[i]) + 1;
        }

        // Allocate memory for the environment
        m_envMem = new char[m_envSize];
        memset(m_envMem, 0, m_envSize);

        // Copy environment to the new memory location
        char* pMem = m_envMem;
        for (int i = 0; envp[i]; ++i) {
            size_t len = strlen(envp[i]) + 1;
            memcpy(pMem, envp[i], len);
            envp[i] = pMem;
            pMem += len;
        }
    }

   
};
