#include "stdafx.h"

#include "gpu/VectorAdaptor.h"
#include "framework/error/Exception.h"
#include "algorithm/projections/back/VoxelSample.h"

namespace ettention
{ 

    template<typename T>
    VectorAdaptor<T>::VectorAdaptor(std::vector<T> other)
        :std::vector<T>(other.begin(), other.end())
    {
    }

    template<typename T>
    VectorAdaptor<T>::VectorAdaptor(unsigned int resolution)
        :std::vector<T>(resolution)
    {
    }

    template<typename T>
    VectorAdaptor<T>::VectorAdaptor(Vec3ui resolution)
        :std::vector<T>(resolution.x)
    {
        if(resolution.y != 1 || resolution.z != 1)
            throw Exception("cannot create GPUMappedVector with 2D or 3D resolution.");
    }

    template<typename T>
    VectorAdaptor<T>::VectorAdaptor(const T* data, size_t dataCount)
        :std::vector<T>(dataCount)
    {
        this->resize(dataCount);
        memcpy(&this[0], data, dataCount * sizeof(T));
    }

    template<typename T>
    VectorAdaptor<T>::~VectorAdaptor()
    {
    }

    template<typename T>
    Vec3ui VectorAdaptor<T>::Resolution3d()
    {
        return Vec3ui((unsigned int) (std::vector<T>::size()* sizeof(T) / sizeof(float)), 1, 1);
    }

    template<typename T>
    unsigned int VectorAdaptor<T>::getResolution() const
    {
        return (VectorAdaptor<T>::ResolutionType)(std::vector<T>::size() * sizeof(T) / sizeof(float));
    }

    template<typename T>
    T* VectorAdaptor<T>::getData()
    {
        return &std::vector<T>::at(0);
    }

    template<typename T>
    const T* VectorAdaptor<T>::getData() const
    {
        return &std::vector<T>::at(0);
    }

    template<typename T>
    std::size_t VectorAdaptor<T>::getByteWidth() const
    {
        return std::vector<T>::size() * sizeof(T);
    }

    template class VectorAdaptor<float>;
    template class VectorAdaptor<int>;
    template class VectorAdaptor<unsigned int>;
    template class VectorAdaptor<unsigned short>;
    template class VectorAdaptor<VoxelSample>;
};