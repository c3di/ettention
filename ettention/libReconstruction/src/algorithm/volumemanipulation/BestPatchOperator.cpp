#include "stdafx.h"
#include "BestPatchOperator.h"
#include "framework/Framework.h"
#include "model/volume/Volume.h"
#include "model/volume/FloatVolume.h"
#include "algorithm/volumemanipulation/SmoothedSquaredDifferencesSumKernel.h"
#include "gpu/GPUMapped.h"

namespace ettention
{
    BestPatchOperator::BestPatchOperator(Framework* framework, GPUMappedVolume* inputVolume, GPUMappedVolume* inputPatch, Vec3ui inputPatchUperLeftCornerCoordinate)
        : framework(framework)
        , volume(inputVolume)
        , patch(inputPatch)
        , inputPatchUperLeftCornerCoordinate(inputPatchUperLeftCornerCoordinate)
    {
        Volume* ssdTmpVolume = new FloatVolume(volume->getProperties().getVolumeResolution(), std::numeric_limits<float>::infinity());
        squaredDifferencesSumVolume = new GPUMappedVolume(framework->getOpenCLStack(), ssdTmpVolume);
        //squaredDifferencesSumKernel = new SmoothedSquaredDifferencesSumKernel(framework, volume, patch, squaredDifferencesSumVolume);
    }

    BestPatchOperator::~BestPatchOperator()
    {
       //delete squaredDifferencesSumKernel;
        delete squaredDifferencesSumVolume->getObjectOnCPU();
        delete squaredDifferencesSumVolume;
    }

    void BestPatchOperator::run()
    {
        squaredDifferencesSumKernel->run();
    }

    void BestPatchOperator::setInputVolume(Volume* inputVolume)
    {
        delete volume;
        volume = new GPUMappedVolume(framework->getOpenCLStack(), inputVolume);
    }

    void BestPatchOperator::setInputVolume(GPUMappedVolume* inputVolume)
    {
        volume = inputVolume;
    }

    void BestPatchOperator::setInputPatch(Volume* inputPatch)
    {
        delete patch;
        patch = new GPUMappedVolume(framework->getOpenCLStack(), inputPatch);
    }

    void BestPatchOperator::setUpperLeftCornerCoordinateOfInputPatch(Vec3ui inputPatchUpperLeftCornerCoordinate)
    {
        inputPatchUperLeftCornerCoordinate = inputPatchUpperLeftCornerCoordinate;
    }

    Volume* BestPatchOperator::getSquaredDifferencesSumVolume()
    {
        squaredDifferencesSumVolume->ensureIsUpToDateOnCPU();
        return squaredDifferencesSumVolume->getObjectOnCPU();
    }

}