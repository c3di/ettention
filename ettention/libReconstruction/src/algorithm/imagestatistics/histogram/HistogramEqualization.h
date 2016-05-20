#pragma once

#include <algorithm>

namespace ettention
{
    class Image;

    class HistogramEqualization
    {
    public:
        static Image* equalizeHistograms(Image* imageForHistogram, Image* imageToBeAdapted);

        template < typename T>
        static std::vector<unsigned int> sortIndices(const T* v, std::size_t numberOfElements);
    };

    template <typename T>
    std::vector<unsigned int> HistogramEqualization::sortIndices(const T* v, std::size_t numberOfElements)
    {
        std::vector<unsigned int> idx(numberOfElements);
        for(unsigned int i = 0; i != numberOfElements; ++i)
        {
            idx[i] = i;
        }

        std::sort(idx.begin(), idx.end(), [&v](unsigned int i1, unsigned int i2) { return v[i1] < v[i2]; });

        return idx;
    }
}

