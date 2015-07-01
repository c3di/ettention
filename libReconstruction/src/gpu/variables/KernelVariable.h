#pragma once
#include <string>
#include <ostream>
#include "framework/math/Vec3.h"

namespace ettention
{
    class KernelVariable
    {
    public:
        typedef std::unique_ptr<KernelVariable> uniquePtr;

        static KernelVariable::uniquePtr CreateFromFloat(const std::string& varName, const float* data, unsigned int floatCount);
        static KernelVariable::uniquePtr CreateFromFloat(const std::string& varName, float value);
        static KernelVariable::uniquePtr CreateFromInt(const std::string& varName, const int* data, unsigned int intCount);
        static KernelVariable::uniquePtr CreateFromUint(const std::string& varName, const unsigned int* data, unsigned int uintCount);
        static KernelVariable::uniquePtr CreateFromVec3f(const std::string& varName, const Vec3f& vec);
        static KernelVariable::uniquePtr CreateFromVec3i(const std::string& varName, const Vec3i& vec);
        static KernelVariable::uniquePtr CreateFromVec3ui(const std::string& varName, const Vec3ui& vec);
        static KernelVariable::uniquePtr CreateFromVec2i(const std::string& varName, const Vec2i& vec);
        static KernelVariable::uniquePtr CreateFromVec2ui(const std::string& varName, const Vec2ui& vec);

        virtual ~KernelVariable();

        void WriteToStream(std::ostream& out) const;
        const void* GetData() const;
        std::size_t GetDataSize() const;

    private:
        enum Type
        {
            TYPE_FLOAT,
            TYPE_INT,
            TYPE_UINT,
        };

        static std::size_t GetTypeBaseSize(Type type);
        static unsigned int GetNextPowerOfTwo(unsigned int val, unsigned int maxExp);

        template<typename _InternalBaseType, typename _BaseType>
        KernelVariable(const std::string& varName, Type type, const _BaseType* data, unsigned int elementCount, _InternalBaseType* dummy)
            : varName(varName)
            , type(type)
            , count(GetNextPowerOfTwo(elementCount, 4))
            , dataSize(count * sizeof(_InternalBaseType))
        {
            this->data = malloc(dataSize);
            for(unsigned int i = 0; i < count; ++i)
            {
                ((_InternalBaseType*)this->data)[i] = i < elementCount ? ((_BaseType*)data)[i] : 0;
            }
        }

        const std::string varName;
        const Type type;
        const unsigned int count;
        void* data;
        std::size_t dataSize;

        template<typename _T>
        void WriteValuesToStream(std::ostream& out) const
        {
            if(count == 1)
            {
                out << *(_T*)data;
            }
            else
            {
                out << "{ ";
                for(unsigned int i = 0; i < count; ++i)
                {
                    out << ((_T*)data)[i] << ", ";
                }
                out << " }";
            }
        }
    };
}