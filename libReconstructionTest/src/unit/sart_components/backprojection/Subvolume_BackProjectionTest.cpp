#include "stdafx.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
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
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "unit/sart_components/backprojection/BackprojectionTest.h"

namespace ettention
{
    class BackProjectionSubvolumesTest : public BackProjectionTest
    {
    public:
        virtual void allocateEmptyVolume(Vec3ui volumeResolution) override
        {
            subVolumeCount = 4;
            volume = new Volume(VolumeProperties(VoxelType::FLOAT_32, VolumeParameterSet(volumeResolution), subVolumeCount), 0.0f);
        }

        unsigned int subVolumeCount;
    };
}

using namespace ettention;
 
TEST_F(BackProjectionSubvolumesTest, Unit_45Direction)
{
    auto image = ImageDeserializer::readImage(testdataDirectory + "back_projection/projection_45.tif");
    residual->setObjectOnCPU(image);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(45.0f));

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
    const OutputFormatParameterSet format(OutputFormatParameterSet::FLOAT_32, OutputFormatParameterSet::ORDER_XZY);
    writer.write(volume, workDirectory + "back_projection/voxel_by_voxel_generated_45_with_subvolumes.mrc", &format);
    StackComparator::assertVolumesAreEqual(workDirectory + "back_projection/voxel_by_voxel_generated_45_with_subvolumes.mrc", testdataDirectory + "back_projection/voxel_by_voxel_reference_45.mrc");

    delete image;
}    
