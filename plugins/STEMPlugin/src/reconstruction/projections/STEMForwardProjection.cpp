#include "stdafx.h"
#include "STEMForwardProjection.h"
#include "Convergent_Beams_Forwardprojection_bin2c.h"
#include "framework/Framework.h"
#include "framework/RandomAlgorithms.h"
#include "framework/error/ProjectionTypeNotSupportedException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/geometry/GeometricSetup.h"
#include "model/volume/GPUMappedVolume.h"

namespace ettention
{
    namespace stem
    {

        STEMForwardProjectionKernel::STEMForwardProjectionKernel(Framework* framework,
                                                                 const Vec2ui& projectionResolution,
                                                                 GeometricSetup* geometricSetup,
                                                                 const Vec3f& unrotatedSourceBase,
                                                                 GPUMappedVolume* volume,
                                                                 int samplesPerBeam)
            : ComputeKernel(framework, Convergent_Beams_Forwardprojection_kernel_SourceCode, "execute", "STEMForwardProjectionKernel")
            , projectionResolution(projectionResolution)
            , geometricSetup(geometricSetup)
            , volume(volume)
            , samplesPerBeam(samplesPerBeam)
            , unrotatedSourceBase(unrotatedSourceBase)
        {
            allocateRandomBuffer();
            output = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
            traversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        }

        STEMForwardProjectionKernel::~STEMForwardProjectionKernel()
        {
            delete traversalLength;
            delete output;
            delete randomSamples;
        }

        void STEMForwardProjectionKernel::setProjectionProperties(const TFProjectionProperties& projectionProperties)
        {
            this->projectionProperties = projectionProperties;
            float focalDistance = projectionProperties.GetFocalDepth() - unrotatedSourceBase.z;
            this->projectionProperties.SetFocalDepth(focalDistance);
        }

        GPUMapped<Image>* STEMForwardProjectionKernel::getOutput() const
        {
            return output;
        }

        GPUMapped<Image>* STEMForwardProjectionKernel::getTraversalLength() const
        {
            return traversalLength;
        }

        void STEMForwardProjectionKernel::prepareKernelArguments()
        {
            implementation->setArgument("projectionResolution", projectionResolution);
            implementation->setArgument("projection", output);
            implementation->setArgument("volume_traversal_length", traversalLength);

            if(volume->getMappedSubVolume()->doesUseImage())
            {
                volume->getMappedSubVolume()->ensureImageIsUpToDate();
                implementation->setArgument("volume", volume->getMappedSubVolume()->getImageOnGPU());
            }
            else
            {
                implementation->setArgument("volume", volume);
            }

            implementation->setArgument("random", randomSamples);
            implementation->setArgument("nSamples", (int)sqrtf((float)samplesPerBeam));
            implementation->setArgument("projectionGeometry", geometricSetup->getProjectionVariableContainer());
            implementation->setArgument("volumeGeometry", geometricSetup->getVolumeVariableContainer());
            implementation->setArgument("convergentBeamsParameter", projectionProperties.getVariableContainer());
        }

        void STEMForwardProjectionKernel::preRun()
        {
            implementation->setGlobalWorkSize(projectionResolution);
        }

        void STEMForwardProjectionKernel::postRun()
        {
            output->markAsChangedOnGPU();
            traversalLength->markAsChangedOnGPU();
        }

        void STEMForwardProjectionKernel::allocateRandomBuffer()
        {
            unsigned int numberOfSamples = samplesPerBeam * 2;
            randomSamples = new GPUMappedVector((CLAbstractionLayer*)abstractionLayer, numberOfSamples);
            RandomAlgorithms::generateRandomNumbersFromMinusOneToOne(numberOfSamples, *randomSamples->getObjectOnCPU());
            randomSamples->markAsChangedOnCPU();
        }
    }
}