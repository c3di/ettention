#pragma once
#include "model/volume/GPUMappedVolume.h"

namespace ettention
{
    class ForwardProjectionKernel;
    class Framework;
    class GeometricSetup;

    class ForwardProjectionOperator
    {
    private:
        Framework* framework;
        ForwardProjectionKernel* kernel;
        GeometricSetup* geometricSetup;
        GPUMappedVolume* volume;
        GPUMappedVolume* priorKnowledgeMask;
        
    public:
        ForwardProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask);
        ~ForwardProjectionOperator();

        void Execute(unsigned int subVolumeIndex);
        ForwardProjectionKernel* GetKernel() const;
        void SetOutput(GPUMapped<Image>* virtualProjection, GPUMapped<Image>* traversalLength);
    };
}