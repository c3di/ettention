#pragma once
#include <vector>

namespace ettention
{
    class VectorAlgorithms
    {
    public:
        typedef union{ float values[2]; struct { float min; float max; }; } tFloatMinMaxPair;

        static tFloatMinMaxPair minMax(const std::vector<float>& vector);

        static void splitString(const std::string& textToSplit, const std::string& delimiter, std::vector<std::string>& result);
        static std::vector<std::string> splitString(const std::string& str, const std::string& delimiter);
    };

}