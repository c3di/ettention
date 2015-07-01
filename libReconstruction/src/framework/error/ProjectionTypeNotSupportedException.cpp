#include "stdafx.h"
#include "framework/error/ProjectionTypeNotSupportedException.h"

namespace ettention
{
    ProjectionTypeNotSupportedException::ProjectionTypeNotSupportedException(const std::string& message)
        : Exception(message)
    {

    }

    ProjectionTypeNotSupportedException::~ProjectionTypeNotSupportedException()
    {

    }
}