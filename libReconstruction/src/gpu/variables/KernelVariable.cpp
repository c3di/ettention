#include "stdafx.h"
#include "KernelVariable.h"
#include "gpu/opencl/CLAbstractionLayer.h"

namespace ettention
{
    KernelVariable::uniquePtr KernelVariable::CreateFromFloat(const std::string& varName, const float* data, unsigned int floatCount)
    {
        cl_float dummy;
        return uniquePtr(new KernelVariable(varName, TYPE_FLOAT, data, floatCount, &dummy));
    }

    KernelVariable::uniquePtr KernelVariable::CreateFromFloat(const std::string& varName, float value)
    {
        return CreateFromFloat(varName, &value, 1);
    }

    KernelVariable::uniquePtr KernelVariable::CreateFromInt(const std::string& varName, const int* data, unsigned int intCount)
    {
        cl_int dummy;
        return uniquePtr(new KernelVariable(varName, TYPE_INT, data, intCount, &dummy));
    }

    KernelVariable::uniquePtr KernelVariable::CreateFromUint(const std::string& varName, const unsigned int* data, unsigned int uintCount)
    {
        cl_uint dummy;
        return uniquePtr(new KernelVariable(varName, TYPE_UINT, data, uintCount, &dummy));
    }

    KernelVariable::uniquePtr KernelVariable::CreateFromVec3f(const std::string& varName, const Vec3f& vec)
    {
        return CreateFromFloat(varName, (float*)&vec, 3);
    }

    KernelVariable::uniquePtr KernelVariable::CreateFromVec3i(const std::string& varName, const Vec3i& vec)
    {
        return CreateFromInt(varName, (int*)&vec, 3);
    }

    KernelVariable::uniquePtr KernelVariable::CreateFromVec3ui(const std::string& varName, const Vec3ui& vec)
    {
        return CreateFromUint(varName, (unsigned int*)&vec, 3);
    }

    KernelVariable::uniquePtr KernelVariable::CreateFromVec2ui(const std::string& varName, const Vec2ui& vec)
    {
        return CreateFromUint(varName, (unsigned int*)&vec, 2);
    }

    KernelVariable::uniquePtr KernelVariable::CreateFromVec2i(const std::string& varName, const Vec2i& vec)
    {
        return CreateFromInt(varName, (int*)&vec, 2);
    }

    unsigned int KernelVariable::GetNextPowerOfTwo(unsigned int val, unsigned int maxExp)
    {
        for(unsigned int exp = 0; exp < maxExp; ++exp)
        {
            if(val <= (unsigned int)1 << exp)
            {
                return 1 << exp;
            }
        }
        return 1 << maxExp;
    }

    std::size_t KernelVariable::GetTypeBaseSize(Type type)
    {
        switch(type)
        {
        case TYPE_FLOAT:
            return sizeof(cl_float);
        case TYPE_INT:
            return sizeof(cl_int);
        case TYPE_UINT:
            return sizeof(cl_uint);
        default:
            return 0;
        }
    }

    KernelVariable::~KernelVariable()
    {
        free(data);
    }

    const void* KernelVariable::GetData() const
    {
        return data;
    }

    std::size_t KernelVariable::GetDataSize() const
    {
        return dataSize;
    }

    void KernelVariable::WriteToStream(std::ostream& out) const
    {
        switch(type)
        {
        case TYPE_FLOAT:
            out << "float";
            if(count != 1)
            {
                out << count;
            }
            out << " " << varName << " = ";
            this->WriteValuesToStream<cl_float>(out);
            break;
        case TYPE_INT:
            out << "float";
            if(count != 1)
            {
                out << count;
            }
            out << " " << varName << " = ";
            this->WriteValuesToStream<cl_int>(out);
            break;
        case TYPE_UINT:
            out << "float";
            if(count != 1)
            {
                out << count;
            }
            out << " " << varName << " = ";
            this->WriteValuesToStream<cl_uint>(out);
            break;
        }
    }
}