#include "stdafx.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/datasource/MRCStack.h"
#include "model/image/ImageComparator.h"
#include "framework/test/ProjectionTestBase.h"

namespace ettention
{
    class ComputeRayLengthTest : public ProjectionTestBase
    {
    public:
        ComputeRayLengthTest()
            : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
        {
        }

        virtual ~ComputeRayLengthTest()
        {
        }

        virtual void SetUp() override
        {
            ProjectionTestBase::SetUp();

            testdataDirectory = std::string(TESTDATA_DIR) + "/data/";
            workDirectory = std::string(TESTDATA_DIR) + "/work/";
            rayLengthImage = new GPUMapped<Image>(clal, projectionResolution);
            computeRayLengthKernel = new ComputeRayLengthKernel(framework, geometricSetup, rayLengthImage);
        }

        virtual void TearDown() override
        {
            delete computeRayLengthKernel;
            delete rayLengthImage;
            ProjectionTestBase::TearDown();
        }

        std::string testdataDirectory, workDirectory;
        GPUMapped<Image>* rayLengthImage;
        ComputeRayLengthKernel* computeRayLengthKernel;
    };
}

using namespace ettention;

TEST_F(ComputeRayLengthTest, Unit) 
{
    computeRayLengthKernel->run();
    ImageSerializer::writeImage(workDirectory + "compute_ray_length/generated_z", computeRayLengthKernel->getOutput(), ImageSerializer::TIFF_GRAY_32);
}                                                           
