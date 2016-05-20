#include "stdafx.h"
#include "ProgressLogger.h"

#ifdef _WINDOWS
    #include <windows.h>
#endif

namespace ettention
{
    ProgressLogger::ProgressLogger()
    {
    }

    ProgressLogger::~ProgressLogger()
    {
    }

    void ProgressLogger::printToConsole(std::string text)
    {
        LOGGER(text);
    }

    void ProgressLogger::setConsoleTitle(std::string title)
    {
#ifdef _WINDOWS
        SetConsoleTitle(title.c_str());
#endif
    }
}