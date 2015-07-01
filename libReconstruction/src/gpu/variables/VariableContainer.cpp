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

    unsigned int VariableContainer::GetVariableCount() const
    {
        return (unsigned int)variables.size();
    }

    void VariableContainer::Push(KernelVariable::uniquePtr var)
    {
        byteWidth += var->GetDataSize();
        variables.push_back(std::move(var));
    }

    std::size_t VariableContainer::GetByteWidth() const
    {
        return byteWidth;
    }

    void VariableContainer::Clear()
    {
        variables.clear();
        byteWidth = 0;
    }

    const KernelVariable::uniquePtr& VariableContainer::Get(unsigned int index) const
    {
        return variables[index];
    }
}
