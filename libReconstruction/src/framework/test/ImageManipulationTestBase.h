#pragma once

#include "framework/Framework.h"
#include "framework/test/CLTestBase.h"

namespace ettention
{
    class Image;
    template<class Image> class GPUMapped;

    class ImageManipulationTestBase : public CLTestBase
    {
    public:
        ImageManipulationTestBase(std::string testCaseName, std::string pathToInputData, std::string pathToOutputData);
        virtual ~ImageManipulationTestBase();

        virtual void OwnSetUp();
        virtual void OwnTearDown();

        virtual std::string makeInputFilePath(std::string imageName, std::string defaultExtension = "tif");
        virtual std::string makeOutputFilePath(std::string imageName, std::string defaultExtension = "tif");

        virtual GPUMapped<Image> *loadImageFileToGPU(std::string imageName);
        virtual void logImageProperties(std::string imageName, GPUMapped<Image> *image);
        virtual void saveImageAsFile(std::string imageName, GPUMapped<Image> *image);

    protected:
        const std::string testCaseName;
        const std::string pathToInputData;
        const std::string pathToOutputData;

    private:
    };
}
