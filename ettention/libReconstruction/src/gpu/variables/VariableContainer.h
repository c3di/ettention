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

        void push(KernelVariable::uniquePtr var);
        void clear();
        unsigned int getVariableCount() const;
        const KernelVariable::uniquePtr& get(unsigned int index) const;
        std::size_t getByteWidth() const;

    protected:
        std::vector<KernelVariable::uniquePtr> variables;
        std::size_t byteWidth;
    };
}