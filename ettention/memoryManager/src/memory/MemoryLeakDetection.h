#pragma once

#ifdef _WIN32
#ifdef COMPILING_DLL
#define DLL_FLAG __declspec(dllexport)
#else
#define DLL_FLAG __declspec(dllimport)
#endif
#else
#define DLL_FLAG
#endif

#ifdef _DEBUG
#include <new>

namespace ettention
{
    DLL_FLAG void* MyAlloc(size_t size, const char* file, int line);
    DLL_FLAG void MyFree(void* data);
}

inline void* operator new(size_t size) { return ettention::MyAlloc(size, 0, 0); }
inline void operator delete(void* data) { ettention::MyFree(data); }

inline void* operator new[](size_t size) { return ettention::MyAlloc(size, 0, 0); }
inline void operator delete[](void* data) { ettention::MyFree(data); }

inline void* operator new(size_t size, const char* file, int line) { return ettention::MyAlloc(size, file, line); }
inline void operator delete(void* data, const char* file, int line) { ettention::MyFree(data); }

inline void* operator new[](size_t size, const char* file, int line) { return ettention::MyAlloc(size, file, line); }
inline void operator delete[](void* data, const char* file, int line) { ettention::MyFree(data); }

#define new new(__FILE__, __LINE__)

#endif
