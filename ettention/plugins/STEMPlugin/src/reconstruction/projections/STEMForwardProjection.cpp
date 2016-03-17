#include "stdafx.h"

#include "STEMForwardProjection.h"
#include "Convergent_Beams_Forwardprojection_bin2c.h"
#include "framework/Framework.h"
#include "framework/RandomAlgorithms.h"
#include "framework/error/ProjectionTypeNotSupportedException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/geometry/GeometricSetup.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/STEMScannerGeometry.h"

namespace ettention
{
    namespace stem
    {

        STEMForwardProjectionKernel::STEMForwardProjectionKernel(Framework* framework,
                                                                 const Vec2ui& projectionResolution,
                                                                 GeometricSetup* geometricSetup,
                                                                 const Vec3f& unrotatedSourceBase,
                                                                 GPUMappedVolume* volume,
                                                                 GPUMappedVolume* priorKnowledgeMask,
                                                                 int samplesPerBeam)
            : ForwardProjectionKernel(framework, Convergent_Beams_Forwardprojection_kernel_SourceCode, "execute", "STEMForwardProjectionKernel", geometricSetup, volume, priorKnowledgeMask, samplesPerBeam)
            , unrotatedSourceBase(unrotatedSourceBase)
        {
            allocateRandomBuffer();
        }

        STEMForwardProjectionKernel::~STEMForwardProjectionKernel()
        {
            delete randomSamples;
        }

        void STEMForwardProjectionKernel::setScannerGeometry(STEMScannerGeometry* geometry)
        {
            this->geometry = (STEMScannerGeometry*) geometry->clone();
            float focalDistance = geometry->getFocalDepth() - unrotatedSourceBase.z;
            this->geometry->setFocalDepth(focalDistance);
        } 

        void STEMForwardProjectionKernel::prepareKernelArguments()
        {
            ForwardProjectionKernel::prepareKernelArguments();    
            implementation->setArgument("random", randomSamples);
            implementation->setArgument("samples", (int) sqrtf( (float) samples ));
        }

        void STEMForwardProjectionKernel::allocateRandomBuffer()
        {
            unsigned int numberOfSamples = samples * 2;
            randomSamples = new GPUMappedVector((CLAbstractionLayer*)abstractionLayer, numberOfSamples);
            RandomAlgorithms::generateRandomNumbersFromMinusOneToOne(numberOfSamples, *randomSamples->getObjectOnCPU());
            randomSamples->markAsChangedOnCPU();
        }
    }
}