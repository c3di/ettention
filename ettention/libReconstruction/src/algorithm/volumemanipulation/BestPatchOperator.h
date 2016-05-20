#pragma once
#include "framework/math/Vec3.h"

namespace ettention
{
    class CLAbstractionLayer;
    class Framework;
    class Volume;
    class SmoothedSquaredDifferencesSumKernel;
    template<typename _T> class GPUMapped;
    class GPUMappedVolume;

    class BestPatchOperator
    {
    public:
        BestPatchOperator(Framework* framework, GPUMappedVolume* inputVolume, GPUMappedVolume* inputPatch, Vec3ui inputPatchUperLeftCornerCoordinate);
        virtual ~BestPatchOperator();

        void run();
        void setInputVolume(Volume* inputVolume);
        void setInputVolume(GPUMappedVolume* inputVolume);
        void setInputPatch(Volume* inputPatch);
        void setUpperLeftCornerCoordinateOfInputPatch(Vec3ui inputPatchUpperLeftCornerCoordinate);
        Volume* getSquaredDifferencesSumVolume();

    private:
        Framework* framework;

        GPUMappedVolume* volume;
        GPUMappedVolume* patch;
        GPUMappedVolume* squaredDifferencesSumVolume;

        Vec3ui inputPatchUperLeftCornerCoordinate;

        SmoothedSquaredDifferencesSumKernel* squaredDifferencesSumKernel;
    };
}
