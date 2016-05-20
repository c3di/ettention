#include "stdafx.h"
#include "ImageManipulationTestBase.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "model/image/ImageComparator.h"

namespace ettention
{
    ImageManipulationTestBase::ImageManipulationTestBase(std::string testCaseName, std::string pathToInputData, std::string pathToOutputData)
        : testCaseName(testCaseName)
        , pathToInputData(pathToInputData + "/")
        , pathToOutputData(pathToOutputData + "/")
    {
        this->OwnSetUp();
    }

    ImageManipulationTestBase::~ImageManipulationTestBase()
    {
        this->OwnTearDown();
    }

    void ImageManipulationTestBase::OwnSetUp()
    {
        std::string logfilename = this->makeOutputFilePath("debug", "log");
        Logger::getInstance().setLogFilename(logfilename);
        Logger::getInstance().activateFileLog(Logger::FORMAT_SIMPLE);
    }

    void ImageManipulationTestBase::OwnTearDown()
    {
        Logger::getInstance().deactivateFileLog();
    }

    std::string ImageManipulationTestBase::makeInputFilePath(std::string fileName, std::string defaultExtension)
    {
        std::string result = this->pathToInputData + fileName;
        if(!defaultExtension.empty())
            result += "." + defaultExtension;

        return result;
    }

    std::string ImageManipulationTestBase::makeOutputFilePath(std::string fileName, std::string defaultExtension)
    {
        std::string result = this->pathToOutputData + this->testCaseName + "_" + fileName;
        if(!defaultExtension.empty())
            result += "." + defaultExtension;

        return result;
    }

    GPUMapped<Image> *ImageManipulationTestBase::loadInputImageFileToGPU(std::string imageName)
    {
        Image *image = ImageDeserializer::readImage(this->makeInputFilePath(imageName));
        auto *result = new GPUMapped<Image>(framework->getOpenCLStack(), image);
        result->takeOwnershipOfObjectOnCPU();
        return result;
    }

    GPUMapped<Image> *ImageManipulationTestBase::loadOutputImageFileToGPU(std::string imageName)
    {
        Image *image = ImageDeserializer::readImage(this->makeOutputFilePath(imageName));
        auto *result = new GPUMapped<Image>(framework->getOpenCLStack(), image);
        result->takeOwnershipOfObjectOnCPU();
        return result;
    }

    void ImageManipulationTestBase::logImageProperties(std::string imageName, GPUMapped<Image> *image)
    {
        std::stringstream sstr;
        image->ensureIsUpToDateOnCPU();
        sstr << "Image " << imageName << "[Range:" << image->getObjectOnCPU()->findMinValue() << "/" << image->getObjectOnCPU()->findMaxValue() << ", Average:" << image->getObjectOnCPU()->computeAverageValue() << "]" << std::endl;
        LOGGER(sstr.str().c_str());
    }

    void ImageManipulationTestBase::saveImageAsFile(std::string imageName, GPUMapped<Image> *image)
    {
        image->ensureIsUpToDateOnCPU();
        ImageSerializer::writeImage(this->makeOutputFilePath(imageName, ""), image, ImageSerializer::TIFF_GRAY_32); // ImageSerializer will add "tiff" extension inside "writeImage" function call.
    }
}