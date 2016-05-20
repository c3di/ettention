#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "model/volume/Volume.h"
#include "model/volume/FloatVolume.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "model/volume/StackComparator.h"
#include "algorithm/volumemanipulation/VolumeBorderSegmentKernel.h"

using namespace ettention;

class VolumeBorderSegmentKernelTest : public TestBase
{
public:
    virtual void applyBorderSegmentation(std::string inFileVolume, std::string outFile)
    {
        Volume *inVolume = VolumeDeserializer::load(inFileVolume, Voxel::DataType::FLOAT_32);

        auto *inBuffer = new GPUMappedVolume(framework->getOpenCLStack(), inVolume);

        auto *kernel = new VolumeBorderSegmentKernel(framework, inBuffer);
        kernel->run();

        MRCWriter writer;
        const OutputParameterSet format(IO_VOXEL_TYPE_UNSIGNED_8, ORDER_XYZ);
        kernel->getOutput()->ensureIsUpToDateOnCPU();
        writer.write(kernel->getOutput()->getObjectOnCPU(), outFile, &format);

        delete kernel;
        delete inBuffer;
        delete inVolume;
    }
};

TEST_F(VolumeBorderSegmentKernelTest, SimpleCase)
{
    std::string filename = "synt_blurred_threshed_segmented.mrc";
    applyBorderSegmentation(std::string(TESTDATA_DIR) + "/data/synthetic_3_parallel/synt_blurred_threshed_00626_02000_10000.mrc", std::string(TESTDATA_DIR) + "/work/synthetic/" + filename);
    ettention::StackComparator::assertVolumesAreEqual(std::string(TESTDATA_DIR) + "/data/synthetic_3_parallel/" + filename, std::string(TESTDATA_DIR) + "/work/synthetic/" + filename);
}