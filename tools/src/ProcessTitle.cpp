#include <tools/ProcessTitle.h>


 void ProcessTitle::setProcTitle(const char* title) {
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