#include "stdafx.h"
#include "HyperStackIndex.h"
#include "framework/error/Exception.h"

namespace ettention
{
    HyperStackIndex::HyperStackIndex()
    {
    }

    HyperStackIndex::HyperStackIndex(const std::vector<unsigned int>& indices)
        : indices(indices)
    {
    }

    HyperStackIndex::HyperStackIndex(unsigned int index0)
        : indices(1)
    {
        indices[0] = index0;
    }

    HyperStackIndex::HyperStackIndex(unsigned int index0, unsigned int index1)
        : indices(2)
    {
        indices[0] = index0;
        indices[1] = index1;
    }

    HyperStackIndex::~HyperStackIndex()
    {
    }

    unsigned int HyperStackIndex::getNumberOfDimensions() const
    {
        return (unsigned int)indices.size();
    }

    unsigned int& HyperStackIndex::operator[](unsigned int i)
    {
        return indices[i];
    }

    const unsigned int& HyperStackIndex::operator[](unsigned int i) const
    {
        return indices[i];
    }

    bool operator<(const HyperStackIndex& a, const HyperStackIndex& b)
    {
        if(a.getNumberOfDimensions() != b.getNumberOfDimensions())
            throw Exception("dimension mismatch while comparing HyperStackIndices");
        for(unsigned int j = 0; j < a.getNumberOfDimensions(); j++)
        {
            if(a[j] < b[j])
                return true;
            if(a[j] > b[j])
                return false;
        }
        return false;
    }

    bool operator==(const HyperStackIndex& a, const HyperStackIndex& b)
    {
        if(a.getNumberOfDimensions() != b.getNumberOfDimensions())
        {
            return false;
        }
        for(unsigned int j = 0; j < a.getNumberOfDimensions(); j++)
        {
            if(a[j] != b[j])
            {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const HyperStackIndex& a, const HyperStackIndex& b)
    {
        return !(a == b);
    }

    std::ostream &operator<<(std::ostream& os, const HyperStackIndex& value)
    {
        switch(value.getNumberOfDimensions())
        {
        case 0:
            os << "?";
            break;
        case 1:
            os << value[0];
            break;
        default:
            os << "(";
            for(unsigned int j = 0; j < value.getNumberOfDimensions(); j++)
            {
                os << value[j] << (j != value.getNumberOfDimensions() - 1 ? ", " : ")");
            }
            break;
        }
        return os;
    }
}