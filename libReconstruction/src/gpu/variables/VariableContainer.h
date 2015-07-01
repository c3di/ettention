#pragma once
#include <vector>
#include "gpu/variables/KernelVariable.h"

namespace ettention
{
    class VariableContainer
    {
    public:
        VariableContainer();
        ~VariableContainer();

        void Push(KernelVariable::uniquePtr var);
        void Clear();
        unsigned int GetVariableCount() const;
        const KernelVariable::uniquePtr& Get(unsigned int index) const;
        std::size_t GetByteWidth() const;

    protected:
        std::vector<KernelVariable::uniquePtr> variables;
        std::size_t byteWidth;
    };
}