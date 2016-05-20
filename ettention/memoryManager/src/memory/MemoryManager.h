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
        void createGlobalListener();
        std::unordered_map<void*, Allocation> allocations;
        std::vector<MemoryListener*> listeners;
        bool locked;
        unsigned int currentId;
        MemoryListener* globalListener;

    protected:
        unsigned int getCurrentState() const;
        void addListener(MemoryListener* listener);
        void deleteListener(MemoryListener* listener);

    public:
        static MemoryManager& getInstance();
        explicit MemoryManager();
        ~MemoryManager();

        void* allocate(size_t size, const char* file = "<unknown_file>", int line = 0);
        void release(void* data);

    };
}