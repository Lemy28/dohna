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

    void setTitle(const char* title) {
        setproctitle(title);
    }

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

    void setproctitle(const char* title) {
        if (title == nullptr) {
            return;
        }

        size_t titleLen = strlen(title);
        size_t totalArgvSize = 0;

        // Calculate the total size of original argv memory
        for (char** pArgv = m_osArgv; *pArgv != nullptr; ++pArgv) {
            totalArgvSize += strlen(*pArgv) + 1;
        }

        // Calculate the total size of argv and environment memory
        size_t totalSize = totalArgvSize + m_envSize;
        if (totalSize <= titleLen) {
            // The title is too long to fit in the original argv memory
            return;
        }

        // Set the remaining command line arguments to null to indicate that there are no more arguments
        m_osArgv[1] = nullptr;

        // Copy the new process title to the beginning of the original argv memory
        char* pTitle = m_osArgv[0];
        strncpy(pTitle, title, titleLen);
        pTitle[titleLen] = '\0';

        // Set the pointer to the first byte after the new process title
        char* pArgvEnd = pTitle + titleLen + 1;

        // Clear the remaining bytes in the original argv memory and environment memory
        size_t clearSize = totalSize - (titleLen + 1);
        memset(pArgvEnd, 0, clearSize);
    }
};
