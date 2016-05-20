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
            forwardKernel = new STEMForwardProjectionKernel(framework, projectionResolution, geometricSetup, unrotatedSourceBase, volume, nullptr, samplesPerBeam);
        }

        STEMForwardProjectionOperator::~STEMForwardProjectionOperator()
        {
            delete forwardKernel;
        }

        GPUMapped<Image>* STEMForwardProjectionOperator::getVirtualProjection()
        {
            return forwardKernel->getVirtualProjection();
        }

        GPUMapped<Image>* STEMForwardProjectionOperator::getRayLength()
        {
            return forwardKernel->getTraversalLength();
        }


        void STEMForwardProjectionOperator::setOutput(GPUMapped<Image>* virtualProjection, GPUMapped<Image>* traversalLength)
        {
            forwardKernel->setOutput(virtualProjection, traversalLength);
        }

        void STEMForwardProjectionOperator::setScannerGeometry(STEMScannerGeometry* geometry)
        {
            forwardKernel->setScannerGeometry(geometry);
        }

        void STEMForwardProjectionOperator::run()
        {
            forwardKernel->getVirtualProjection()->getBufferOnGPU()->clear();
            forwardKernel->getTraversalLength()->getBufferOnGPU()->clear();
            for(unsigned int i = 0; i < volume->getObjectOnCPU()->getProperties().getSubVolumeCount(); i++)
            {
                geometricSetup->setCurrentSubVolumeIndex(i);
                volume->setCurrentSubvolumeIndex(i);
                volume->ensureIsUpToDateOnGPU();
                forwardKernel->run();
            }
        }
    }
}