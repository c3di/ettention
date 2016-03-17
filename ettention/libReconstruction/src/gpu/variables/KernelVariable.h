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

        static KernelVariable::uniquePtr createFromFloat(const std::string& varName, const float* data, unsigned int floatCount);
        static KernelVariable::uniquePtr createFromFloat(const std::string& varName, float value);
        static KernelVariable::uniquePtr createFromInt(const std::string& varName, const int* data, unsigned int intCount);
        static KernelVariable::uniquePtr createFromUint(const std::string& varName, const unsigned int* data, unsigned int uintCount);
        static KernelVariable::uniquePtr createFromVec3f(const std::string& varName, const Vec3f& vec);
        static KernelVariable::uniquePtr createFromVec3i(const std::string& varName, const Vec3i& vec);
        static KernelVariable::uniquePtr createFromVec3ui(const std::string& varName, const Vec3ui& vec);
        static KernelVariable::uniquePtr createFromVec2i(const std::string& varName, const Vec2i& vec);
        static KernelVariable::uniquePtr createFromVec2ui(const std::string& varName, const Vec2ui& vec);

        virtual ~KernelVariable();

        void writeToStream(std::ostream& out) const;
        const void* getData() const;
        std::size_t getDataSize() const;

    private:
        enum Type
        {
            TYPE_FLOAT,
            TYPE_INT,
            TYPE_UINT,
        };

        static std::size_t getTypeBaseSize(Type type);
        static unsigned int getNextPowerOfTwo(unsigned int val, unsigned int maxExp);

        template<typename _InternalBaseType, typename _BaseType>
        KernelVariable(const std::string& varName, Type type, const _BaseType* data, unsigned int elementCount, _InternalBaseType* dummy)
            : varName(varName)
            , type(type)
            , count(getNextPowerOfTwo(elementCount, 4))
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
        void writeValuesToStream(std::ostream& out) const
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