#include "stdafx.h"
#include "STEMForwardProjectionOperator.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "model/geometry/GeometricSetup.h"

namespace ettention
{
    namespace stem
    {
        STEMForwardProjectionOperator::STEMForwardProjectionOperator(Framework* framework,
                                                                     const Vec2ui& projectionResolution,
                                                                     GeometricSetup* geometricSetup,
                                                                     const Vec3f& unrotatedSourceBase,
                                                                     GPUMappedVolume* volume,
                                                                     unsigned int samplesPerBeam)
            : projectionResolution(projectionResolution)
            , geometricSetup(geometricSetup)
            , volume(volume)
            , samplesPerBeam(samplesPerBeam)
        {
            forwardKernel = new STEMForwardProjectionKernel(framework, projectionResolution, geometricSetup, unrotatedSourceBase, volume, samplesPerBeam);
        }

        STEMForwardProjectionOperator::~STEMForwardProjectionOperator()
        {
            delete forwardKernel;
        }

        GPUMapped<Image>* STEMForwardProjectionOperator::getOutput()
        {
            return forwardKernel->getOutput();
        }

        GPUMapped<Image>* STEMForwardProjectionOperator::getRayLength()
        {
            return forwardKernel->getTraversalLength();
        }

        void STEMForwardProjectionOperator::SetProjectionProperties(const TFProjectionProperties& projectionProperties)
        {
            forwardKernel->setProjectionProperties(projectionProperties);
        }

        void STEMForwardProjectionOperator::run()
        {
            forwardKernel->getOutput()->getBufferOnGPU()->clear();
            forwardKernel->getTraversalLength()->getBufferOnGPU()->clear();
            for(unsigned int i = 0; i < volume->getObjectOnCPU()->Properties().GetSubVolumeCount(); i++)
            {
                geometricSetup->setCurrentSubVolumeIndex(i);
                volume->setCurrentSubvolumeIndex(i);
                volume->ensureIsUpToDateOnGPU();
                forwardKernel->run();
            }
        }
    }
}