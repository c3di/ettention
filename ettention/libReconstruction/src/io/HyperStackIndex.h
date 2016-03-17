#pragma once

#include <vector>

namespace ettention
{
    class HyperStackIndex
    {
    public:
        HyperStackIndex();
        HyperStackIndex(const std::vector<unsigned int>& indices);
        HyperStackIndex(unsigned int index0);
        HyperStackIndex(unsigned int index0, unsigned int index1);
        ~HyperStackIndex();

        unsigned int getNumberOfDimensions() const;

        unsigned int& operator[](unsigned int i);
        const unsigned int& operator[](unsigned int i) const;

    private:
        std::vector<unsigned int> indices;
    };

    bool operator<(const HyperStackIndex& a, const HyperStackIndex& b);
    bool operator==(const HyperStackIndex& a, const HyperStackIndex& b);
    bool operator!=(const HyperStackIndex& a, const HyperStackIndex& b);

    std::ostream &operator<<(std::ostream& os, const HyperStackIndex& value);

    class HyperStackIndexHash
    {
    public:
        inline std::size_t operator()(const HyperStackIndex& index) const
        {
            std::size_t hash = 0;
            for(unsigned int i = 0; i < index.getNumberOfDimensions(); ++i)
            {
                hash += 1024 * hash + index[i];
            }
            return hash;
        }
    };
}