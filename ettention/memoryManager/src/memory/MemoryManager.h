#pragma once
#include <unordered_map>
#include <vector>
#include "MemoryLeakDetection.h"

namespace ettention
{
    class MemoryListener;

    class DLL_FLAG MemoryManager
    {
        friend class MemoryListener;
    public:
        struct Allocation
        {
            const char* file;
            int line;
            size_t size;
            void* data;
            unsigned int id;
        };
        

    private:
        void CreateGlobalListener();
        std::unordered_map<void*, Allocation> allocations;
        std::vector<MemoryListener*> listeners;
        bool locked;
        unsigned int currentId;
        MemoryListener* globalListener;

    protected:
        unsigned int GetCurrentState() const;
        void AddListener(MemoryListener* listener);
        void DeleteListener(MemoryListener* listener);

    public:
        static MemoryManager& GetInstance();
        explicit MemoryManager();
        ~MemoryManager();

        void* Alloc(size_t size, const char* file = "<unknown_file>", int line = 0);
        void Free(void* data);

    };
}