#include <sstream>
#include <algorithm>
#include "MemoryListener.h"

namespace ettention
{
    MemoryListener::MemoryListener(const std::string& name)
        : refState(MemoryManager::GetInstance().GetCurrentState())
        , name(name)
    {
        globalAllocationInfo.allocatedBytes = 0;
        globalAllocationInfo.deallocatedBytes = 0;
        globalAllocationInfo.allocationCount = 0;
        globalAllocationInfo.deallocationCount = 0;
        MemoryManager::GetInstance().AddListener(this);
    }

    MemoryListener::~MemoryListener()
    {
        MemoryManager::GetInstance().DeleteListener(this);
    }

    void MemoryListener::OnAllocation(const MemoryManager::Allocation& allocation)
    {
        std::stringstream location;
        location << allocation.file << "(" << allocation.line << ")";
        auto findIt = allocatedMemory.find(location.str());
        if(findIt != allocatedMemory.end())
        {
            findIt->second.allocatedBytes += allocation.size;
            ++findIt->second.allocationCount;
            findIt->second.pendingAllocations.insert(allocation.data);
            allocationIds[allocation.data] = findIt->second.nextAllocationId++;
        }
        else
        {
            AllocationInfo info = { allocation.size, 0, 1, 0, 1, std::unordered_set<void*>(), };
            info.pendingAllocations.insert(allocation.data);
            allocatedMemory[location.str()] = info;
            allocationIds[allocation.data] = 0;
        }
        ++globalAllocationInfo.allocationCount;
        globalAllocationInfo.allocatedBytes += allocation.size;
    }

    void MemoryListener::OnDeallocation(const MemoryManager::Allocation& allocation)
    {
        std::stringstream location;
        location << allocation.file << "(" << allocation.line << ")";
        auto findIt = allocatedMemory.find(location.str());
        if(findIt != allocatedMemory.end())
        {
            findIt->second.deallocatedBytes += allocation.size;
            ++findIt->second.deallocationCount;
            findIt->second.pendingAllocations.erase(allocation.data);
            globalAllocationInfo.deallocatedBytes += allocation.size;
            ++globalAllocationInfo.deallocationCount;
            auto findIdIt = allocationIds.find(allocation.data);
            if(findIdIt != allocationIds.end())
            {
                allocationIds.erase(findIdIt);
            }
        }
        else
        {
            throw std::runtime_error("Called OnDeallocation() with an allocation that was not previously given to OnAllocation()!");
        }
    }

    std::string FormatBytes(std::size_t sizeInBytes)
    {
        std::stringstream stream;
        if(sizeInBytes > 2 << 20)
        {
            stream << (sizeInBytes / (1 << 20)) << " MiB (" << sizeInBytes << " byte(s))";
        }
        else if(sizeInBytes > 2 << 10)
        {
            stream << (sizeInBytes / (1 << 10)) << " KiB (" << sizeInBytes << " byte(s))";
        }
        else
        {
            stream << sizeInBytes << " byte(s)";
        }
        return stream.str();
    }

    void MemoryListener::PrintSummary(std::ostream& out)
    {
        out << "+- MemoryReport -- " << name << " ";
        for(auto i = name.size(); i < 16; ++i)
        {
            out << "-";
        }
        out << "--+" << std::endl;
        out << "+- - - Summary - - - - - -+" << std::endl << std::endl;
        out << " " << FormatBytes(globalAllocationInfo.allocatedBytes) << " allocated by " << globalAllocationInfo.allocationCount << " allocation(s)." << std::endl;
        out << " " << FormatBytes(globalAllocationInfo.deallocatedBytes) << " deallocated by " << globalAllocationInfo.deallocationCount << " deallocation(s)." << std::endl;
        out << " " << FormatBytes(globalAllocationInfo.allocatedBytes - globalAllocationInfo.deallocatedBytes) << " leaked by " << (globalAllocationInfo.allocationCount - globalAllocationInfo.deallocationCount) << " missing deallocation(s)." << std::endl << std::endl;
        if(globalAllocationInfo.allocatedBytes != globalAllocationInfo.deallocatedBytes)
        {
            out << "+- - - Details - - - - - -+" << std::endl << std::endl;
            std::vector<std::string> leaks;
            for(auto it = allocatedMemory.begin(); it != allocatedMemory.end(); ++it)
            {
                if(it->second.allocatedBytes != it->second.deallocatedBytes)
                {
                    std::stringstream leak;
                    leak << " " << it->first << std::endl;
                    leak << " " << it->second.allocationCount << " allocation(s), " << it->second.deallocationCount << " deallocation(s), " << (it->second.allocationCount - it->second.deallocationCount) << " missing deallocation(s) which leaked " << FormatBytes(it->second.allocatedBytes - it->second.deallocatedBytes) << "." << std::endl;
                    leak << " " << "Missing deallocations:";
                    std::vector<unsigned int> ids;
                    for(auto idIt = it->second.pendingAllocations.begin(); idIt != it->second.pendingAllocations.end(); ++idIt)
                    {
                        ids.push_back(allocationIds[*idIt]);
                    }
                    std::sort(ids.begin(), ids.end());
                    for(auto idIt = ids.begin(); idIt != ids.end(); ++idIt)
                    {
                        leak << " #" << *idIt;
                    }
                    leak << std::endl;
                    leaks.push_back(leak.str());
                }
            }
            std::sort(leaks.begin(), leaks.end());
            for(auto it = leaks.begin(); it != leaks.end(); ++it)
            {
                out << *it << std::endl;
            }
        }
        out << "+-------------------";
        for(auto i = 0; i < std::max(16, (int)name.size()); ++i)
        {
            out << "-";
        }
        out << "--+" << std::endl << std::endl;
    }

    bool MemoryListener::IsMemoryLeaked() const
    {
        return globalAllocationInfo.allocatedBytes != globalAllocationInfo.deallocatedBytes;
    }
}