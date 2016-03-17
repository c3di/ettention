#include "stdafx.h"
#include "setup/ParameterStorage.h"

namespace ettention
{
    ParameterStorage::ParameterStorage()
    {
    }

    ParameterStorage::~ParameterStorage()
    {
    }

    bool ParameterStorage::parameterExists(std::string name) const
    {
        return propertyTree.get_optional<std::string>(name) != boost::none;
    }
}