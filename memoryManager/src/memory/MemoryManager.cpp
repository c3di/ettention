#include <iostream>
#include "MemoryManager.h"
#include "MemoryListener.h"
#ifdef _WINDOWS
#include <sstream>
#include <windows.h>
#endif

namespace ettention
{
    MemoryManager::MemoryManager() :
        locked(true),
        currentId(0)
    {
        locked = false;
    }

    MemoryManager::~MemoryManager()
    {
#ifdef _WINDOWS
        std::stringstream str;
        globalListener->PrintSummary(str);
        OutputDebugStringA(str.str().c_str());
#else
        globalListener->PrintSummary(std::cout);
#endif
        locked = true;
        delete globalListener;
    }

    MemoryManager& MemoryManager::GetInstance()
    {
        static MemoryManager instance;
        static int entryCounter = 0;
        if(entryCounter == 0)
        {
            entryCounter++;
            instance.CreateGlobalListener();
        }
        return instance;
    }

    void MemoryManager::CreateGlobalListener()
    {
        locked = true;
        globalListener = new MemoryListener("global");
        locked = false;
    }

    unsigned int MemoryManager::GetCurrentState() const
    {
        return currentId;
    }

    void* MemoryManager::Alloc(size_t size, const char* file, int line)
    {
        void* data = malloc(size);
        if(!locked)
        {
            locked = true;
            Allocation allocation = { file, line, size, data, currentId++, };
            allocations[data] = allocation;
            for(auto it = listeners.begin(); it != listeners.end(); ++it)
            {
                if(allocation.id >= (*it)->refState)
                {
                    (*it)->OnAllocation(allocation);
                }
            }
            locked = false;
        }
        return data;
    }

    void MemoryManager::Free(void* data)
    {
        if(!locked)
        {
            locked = true;
            auto findIt = allocations.find(data);
            if(findIt != allocations.end())
            {
                for(auto it = listeners.begin(); it != listeners.end(); ++it)
                {
                    if(findIt->second.id >= (*it)->refState)
                    {
                        (*it)->OnDeallocation(findIt->second);
                    }
                }
                allocations.erase(findIt);
            }
            locked = false;
        }
        free(data);
    }

    void MemoryManager::AddListener(MemoryListener* listener)
    {
        listeners.push_back(listener);
    }

    void MemoryManager::DeleteListener(MemoryListener* listener)
    {
        for(auto it = listeners.begin(); it != listeners.end(); ++it)
        {
            if(*it == listener)
            {
                listeners.erase(it);
                return;
            }
        }
    }
}