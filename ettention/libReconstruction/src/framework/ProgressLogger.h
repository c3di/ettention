#pragma once
#include "framework/Logger.h"

namespace ettention
{
    class ProgressLogger
    {
    public:
        ProgressLogger();
        ~ProgressLogger();

    protected:
        void printToConsole(std::string text);
        void setConsoleTitle(std::string title);
    };
}
