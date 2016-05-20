#include "stdafx.h"
#include "gpu/variables/VariableContainer.h"

namespace ettention
{
    VariableContainer::VariableContainer()
        : byteWidth(0)
    {

    }

    VariableContainer::~VariableContainer()
    {
        
    }

    unsigned int VariableContainer::getVariableCount() const
    {
        return (unsigned int)variables.size();
    }

    void VariableContainer::push(KernelVariable::uniquePtr var)
    {
        byteWidth += var->getDataSize();
        variables.push_back(std::move(var));
    }

    std::size_t VariableContainer::getByteWidth() const
    {
        return byteWidth;
    }

    void VariableContainer::clear()
    {
        variables.clear();
        byteWidth = 0;
    }

    const KernelVariable::uniquePtr& VariableContainer::get(unsigned int index) const
    {
        return variables[index];
    }
}
