#pragma once

#include <vector>
#include "framework/math/Vec3.h"

namespace ettention
{

    template<typename T>
    class VectorAdaptor : public std::vector<T>
    {
    public:
        typedef unsigned int ResolutionType;

        VectorAdaptor(std::vector<T> other);
        VectorAdaptor(unsigned int resolution);
        VectorAdaptor(Vec3ui resolution);
        VectorAdaptor(const T* data, size_t dataCount);

        ~VectorAdaptor();

        Vec3ui Resolution3d();
        unsigned int getResolution() const;
        T* getData();
        const T* getData() const;

        std::size_t getByteWidth() const;
    };
};
