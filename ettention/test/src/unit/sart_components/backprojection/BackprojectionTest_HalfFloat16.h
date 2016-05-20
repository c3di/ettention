#pragma once
#include "setup/parameterset/HardwareParameterSet.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/volumemanipulation/AccumulateVolumeKernel.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/datasource/MRCStack.h"
#include "framework/VectorAlgorithms.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "framework/test/BackProjectionTestBase.h"

namespace ettention
{
    class BackProjectionHalfFloat16Test : public BackProjectionTestBase
    {
    public:
        BackProjectionHalfFloat16Test()
            : BackProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
        {
        }

        ~BackProjectionHalfFloat16Test()
        {
        }

        virtual void allocateEmptyVolume(Vec3ui volumeResolution) override;
        virtual void InitOptimizationParameter() override;
    };
}