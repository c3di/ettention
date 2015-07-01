#include "MemoryLeakDetection.h"
#include "MemoryManager.h"

namespace ettention
{
    DLL_FLAG void* MyAlloc(size_t size, const char* file, int line)
    {
        return file ? MemoryManager::GetInstance().Alloc(size, file, line) : malloc(size);
    }

    DLL_FLAG void MyFree(void* data)
    {
        MemoryManager::GetInstance().Free(data);
    }
}