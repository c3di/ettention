#pragma once
#include "MemoryManager.h"
#include <unordered_set>

namespace ettention
{
    class DLL_FLAG MemoryListener final
    {
        friend class MemoryManager;
    public:
        MemoryListener(const std::string& name);
        ~MemoryListener();

        void PrintSummary(std::ostream& out);
        bool IsMemoryLeaked() const;

    protected:
        const unsigned int refState;

        void OnAllocation(const MemoryManager::Allocation& allocation);
        void OnDeallocation(const MemoryManager::Allocation& allocation);
                
    private:
        struct AllocationInfo
        {
            std::size_t allocatedBytes;
            std::size_t deallocatedBytes;
            unsigned int allocationCount;
            unsigned int deallocationCount;
            unsigned int nextAllocationId;
            std::unordered_set<void*> pendingAllocations;
        };
        typedef std::string Location;
        
        const std::string name;
        std::unordered_map<Location, AllocationInfo> allocatedMemory;
        std::unordered_map<void*, unsigned int> allocationIds;
        AllocationInfo globalAllocationInfo;

    };
}