#include "stdafx.h"
#include "setup/parameterset/HardwareParameterSet.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/volumemanipulation/AccumulateVolumeKernel.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/datasource/MRCStack.h"
#include "framework/VectorAlgorithms.h"
#include "framework/test/BackProjectionTestBase.h"
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "model/volume/FloatVolume.h"

namespace ettention
{
    class BackProjectionSubvolumesTest : public BackProjectionTestBase
    {
    public:
        BackProjectionSubvolumesTest()
            : BackProjectionTestBase(std::string(TESTDATA_DIR) + "/data/back_projection/", std::string(TESTDATA_DIR) + "/work/back_projection/")
        {
        }

        ~BackProjectionSubvolumesTest()
        {
        }

        virtual void allocateEmptyVolume(Vec3ui volumeResolution) override
        {
            subVolumeCount = 4;
            volume = new FloatVolume(volumeResolution, 0.0f, subVolumeCount);
        }

        unsigned int subVolumeCount;
    };
}

using namespace ettention;
 
TEST_F(BackProjectionSubvolumesTest, Unit_45Direction)
{
    auto image = ImageDeserializer::readImage(testdataDirectory + "projection_45.tif");
    residual->setObjectOnCPU(image);
    geometricSetup->setScannerGeometry(satRotator->createRotatedScannerGeometry(45.0f));

    for(unsigned int subVolumeIndex = 0; subVolumeIndex < subVolumeCount; subVolumeIndex++)
    {
        geometricSetup->setCurrentSubVolumeIndex(subVolumeIndex);
        accumulatedVolume->setCurrentSubvolumeIndex(subVolumeIndex);

        computeRayLength->run();
        performBackProjection();
        backProjectKernel->getOutput()->ensureIsUpToDateOnCPU();
        backProjectKernel->getOutput()->getBufferOnGPU()->clear();
    }
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XZY);
    writer.write(volume, workDirectory + "voxel_by_voxel_generated_45_with_subvolumes.mrc", &format);
    StackComparator::assertVolumesAreEqual(workDirectory + "voxel_by_voxel_generated_45_with_subvolumes.mrc", testdataDirectory + "voxel_by_voxel_reference_45.mrc");

    delete image;
}    
