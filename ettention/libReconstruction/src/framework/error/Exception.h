#pragma once

#include <iostream>
#include <string>

#ifdef _NOEXCEPT
#define noexcept _NOEXCEPT
#endif

namespace ettention
{
    class Exception : public std::exception
    {
    public:
        Exception(const std::string& message = "");
        virtual ~Exception();

        void setMessage(const std::string& message);
        const char* what() const noexcept;

    private:
        std::string message;
    };
}

// simple way to throw an exception with a message containing a number or other hard to stringify data
// example:
//
// int i = 7;
// ETTENTION_THROW_EXCEPTION("Error #" << i << " occured!");
//

#define ETTENTION_THROW_EXCEPTION(_message) do \
{ \
    std::stringstream message; \
    message << _message; \
    throw Exception(message.str()); \
} while(0)
