#include "stdafx.h"
#include "framework/test/TestBase.h"
#include "model/volume/Volume.h"
#include "model/volume/FloatVolume.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "algorithm/volumemanipulation/SmoothedSquaredDifferencesSumKernel.h"
#include "io/serializer/MRCWriter.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "model/volume/StackComparator.h"

using namespace ettention;

class SmoothedSquaredDifferencesSumKernelTest : public TestBase
{
public:
    virtual void applySSDKernel(std::string inFileVolume, std::string inFilePatch, std::string outFile)
    {
        Volume* inVolume = VolumeDeserializer::load(inFileVolume, Voxel::DataType::FLOAT_32);
        Volume* inPatch = VolumeDeserializer::load(inFilePatch, Voxel::DataType::FLOAT_32);
        //Volume* inPatch = new Volume(Vec3ui(3, 3, 3), 0.0F);
        Volume* SSDVolume = new FloatVolume(inVolume->getProperties().getVolumeResolution(), std::numeric_limits<float>::infinity());

        auto* bufferVolume = new GPUMappedVolume(framework->getOpenCLStack(), inVolume);
        auto* bufferPatch = new GPUMappedVolume(framework->getOpenCLStack(), inPatch);
        auto* bufferSSD = new GPUMappedVolume(framework->getOpenCLStack(), SSDVolume);

        auto* kernel = new SmoothedSquaredDifferencesSumKernel(framework, bufferVolume, Vec3ui(0,0,0), bufferPatch, bufferSSD, 0.0f);
        kernel->run();

        MRCWriter writer;
        const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XYZ);
        kernel->getSmoothedSquaredDifferencesSumVolume()->ensureIsUpToDateOnCPU();
        writer.write(kernel->getSmoothedSquaredDifferencesSumVolume()->getObjectOnCPU(), outFile, &format);

        delete kernel;
        delete bufferSSD;
        delete bufferPatch;
        delete bufferVolume;
        delete SSDVolume;
        delete inPatch;
        delete inVolume;
    }
};
                                            
TEST_F(SmoothedSquaredDifferencesSumKernelTest, DISABLED_applySSDKernel)
{
    applySSDKernel(std::string(TESTDATA_DIR) + "/data/ssd/inputVolume.mrc", std::string(TESTDATA_DIR) + "/data/ssd/inputPatch.mrc", std::string(TESTDATA_DIR) + "/work/ssd/smoothedSSDVolume.mrc");
    ettention::StackComparator::assertVolumesAreEqual(std::string(TESTDATA_DIR) + "/work/ssd/smoothedSSDVolume.mrc", std::string(TESTDATA_DIR) + "/data/ssd/smoothedSSDVolume.mrc");
}