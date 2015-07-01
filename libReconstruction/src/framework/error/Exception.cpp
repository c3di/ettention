#include "stdafx.h"
#include "framework/error/Exception.h"

namespace ettention
{
    Exception::Exception(const std::string& message)
        : message(message)
    {
    }

    Exception::~Exception() throw()
    {
    }

    void Exception::setMessage(const std::string& message)
    {
        this->message = message;
    }

    const char* Exception::what() const throw()
    {
        return message.c_str();
    }
}