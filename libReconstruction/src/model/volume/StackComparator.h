#pragma once

#include <string>
#include "io/datasource/ImageStackDatasource.h"

namespace ettention
{
    class Volume;

    class StackComparator
    {
    public:
        StackComparator(ImageStackDatasource& firstInputStack, ImageStackDatasource& secondInputStack);
        ~StackComparator();

        unsigned int getSliceIndexWithMaxRMS();

        float getMaxRMS();

        float getRMSOfSlice(unsigned int sliceIndex);

        static float getRMSBetweenVolumes(std::string firstVolume, std::string secondVolume);
        static void assertVolumesAreEqual(std::string firstVolume, std::string secondVolume);

        static float getRMSBetweenVolumes(Volume* firstVolume, Volume* secondVolume);
        static void assertVolumesAreEqual(Volume* firstVolume, Volume* secondVolume);

    private:
        ImageStackDatasource& firstStack;
        ImageStackDatasource& secondStack;
        static const float acceptableRMSError;

    };
}

