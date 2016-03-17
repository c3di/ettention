#include "stdafx.h"
#include "StackComparator.h"
#include <boost/format.hpp>
#include "framework/error/Exception.h"
#include "framework/Logger.h"
#include "model/volume/Volume.h"
#include "io/datasource/MRCStack.h"
#include "framework/evaluation/RootMeanSquareError.h"

namespace ettention
{
    const float StackComparator::acceptableRMSError = 0.011f;

    StackComparator::StackComparator(ImageStackDatasource& firstInputStack, ImageStackDatasource& secondInputStack) :
        firstStack(firstInputStack),
        secondStack(secondInputStack)
    {
    }

    StackComparator::~StackComparator()
    {

    }

    float StackComparator::getRMSOfSlice(unsigned int sliceIndex)
    {
        auto sliceDataFromFirstStack = firstStack.getProjectionImage(sliceIndex);
        auto sliceDataFromSecondStack = secondStack.getProjectionImage(sliceIndex);
        float rmsError = RootMeanSquareError::computeRMSError(sliceDataFromFirstStack->getData(), sliceDataFromSecondStack->getData(), sliceDataFromFirstStack->getPixelCount());
        return rmsError;
    }

    unsigned int StackComparator::getSliceIndexWithMaxRMS()
    {
        float rmsError = 0.f;
        float maxRMSError = -1.f;
        unsigned int sliceIndexWithMaxRMS = 0;

        for(unsigned int i = 0; i<firstStack.getNumberOfProjections(); i++)
        {
            auto sliceDataFromFirstStack = firstStack.getProjectionImage(i);
            auto sliceDataFromSecondStack = secondStack.getProjectionImage(i);
            rmsError = RootMeanSquareError::computeRMSError(sliceDataFromFirstStack->getData(), sliceDataFromSecondStack->getData(), sliceDataFromFirstStack->getPixelCount());

            if(rmsError>maxRMSError)
            {
                maxRMSError = rmsError;
                sliceIndexWithMaxRMS = i;
            }
        }

        return sliceIndexWithMaxRMS;
    }

    float StackComparator::getMaxRMS()
    {
        float rmsError = 0.f;
        float maxRMSError = -1.f;

        for(unsigned int i = 0; i < firstStack.getNumberOfProjections(); i++)
        {
            auto sliceDataFromFirstStack = firstStack.getProjectionImage(i);
            auto sliceDataFromSecondStack = secondStack.getProjectionImage(i);
            rmsError = RootMeanSquareError::computeRMSError(sliceDataFromFirstStack->getData(), sliceDataFromSecondStack->getData(), sliceDataFromFirstStack->getPixelCount());
            if(rmsError>maxRMSError)
            {
                maxRMSError = rmsError;
            }
        }
        return maxRMSError;
    }

    float StackComparator::getRMSBetweenVolumes(std::string firstVolume, std::string secondVolume)
    {
        ImageStackDatasource* firstStack = new MRCStack(firstVolume, false);
        ImageStackDatasource* secondStack = new MRCStack(secondVolume, false);
        if(firstStack->getResolution() != secondStack->getResolution() ||
           firstStack->getNumberOfProjections() != secondStack->getNumberOfProjections())
        {
            throw std::runtime_error("illegal comparison of volumes with different size");
        }
        StackComparator comparator(*firstStack, *secondStack);
        const float rms = comparator.getMaxRMS();
        delete secondStack;
        delete firstStack;
        return rms;
    }

    void StackComparator::assertVolumesAreEqual(std::string firstVolume, std::string secondVolume)
    {
        const float rms = getRMSBetweenVolumes(firstVolume, secondVolume);
        if(rms > acceptableRMSError)
        {
            std::string message = (boost::format("volumes\n\t%1% and\n\t%2%\n\tdifference: %3%") % firstVolume % secondVolume % rms).str();
            LOGGER_ERROR(message)
                throw Exception(message);
        }
    }

    float StackComparator::getRMSBetweenVolumes(Volume* firstVolume, Volume* secondVolume)
    {
        if(firstVolume->getProperties().getVolumeResolution() != secondVolume->getProperties().getVolumeResolution())
            throw std::runtime_error("illegal comparison of volumes with different size");

        auto dataOne = firstVolume->convertToFloat();
        auto dataTwo = secondVolume->convertToFloat();

        return RootMeanSquareError::computeRMSError(dataOne.get(), dataTwo.get(), firstVolume->getProperties().getVolumeVoxelCount());
    }

    void StackComparator::assertVolumesAreEqual(Volume* firstVolume, Volume* secondVolume)
    {
        const float rms = getRMSBetweenVolumes(firstVolume, secondVolume);
        //std::cout << rms << std::endl;
        if(rms > acceptableRMSError)
        {
            std::string message = (boost::format("volumes differ by %1%") % rms).str();
            LOGGER_ERROR(message)
                throw Exception(message);
        }
    }
}