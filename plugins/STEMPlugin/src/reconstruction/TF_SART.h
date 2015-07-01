#pragma once
#include "gpu/opencl/CLAbstractionLayer.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/projectionset/ProjectionSetMaxAngle.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "algorithm/imagemanipulation/CompareKernel.h"
#include "algorithm/longobjectcompensation/VirtualProjectionKernel.h"
#include "algorithm/volumemanipulation/AccumulateVolumeKernel.h"
#include "algorithm/imagemanipulation/RMSKernel.h"
#include "algorithm/imagemanipulation/fft/FFTForwardKernel.h"
#include "algorithm/reconstruction/ReconstructionAlgorithm.h"
#include "reconstruction/operators/TFS_BackProjection_Operator.h"

#include "alignment/autofocus/AutomaticFocusComputation.h"
#include "reconstruction/operators/STEMForwardProjectionOperator.h"
#include "reconstruction/TF_ART.h"

namespace ettention
{
    namespace stem
    {
        class PLUGIN_API TF_SART : public TF_ART
        {
        public:
            TF_SART(Framework* framework);
            ~TF_SART();

        protected:
            void handleOneIteration() override;
        };
    }
}
