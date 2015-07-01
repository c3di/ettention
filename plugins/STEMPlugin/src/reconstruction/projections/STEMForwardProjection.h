#pragma once
#include "gpu/GPUMapped.h"
#include "gpu/ComputeKernel.h"
#include "model/TFProjectionProperties.h"
#include "model/volume/Volume.h"
#include "model/volume/SubVolume.h"

namespace ettention
{
    class GPUMappedVolume;
    class GeometricSetup;

    namespace stem
    {
        class PLUGIN_API STEMForwardProjectionKernel : public ComputeKernel
        {
        public:
            STEMForwardProjectionKernel(Framework* framework, const Vec2ui& projectionResolution, GeometricSetup* geometricSetup, const Vec3f& unrotatedSourceBase, GPUMappedVolume* volume, int samplesPerBeam);
            virtual ~STEMForwardProjectionKernel();

            void setProjectionProperties(const TFProjectionProperties& projectionProperties);
            GPUMapped<Image>* getOutput() const;
            GPUMapped<Image>* getTraversalLength() const;

        protected:
            void prepareKernelArguments() override;
            void preRun() override;
            void postRun() override;

        private:
            void allocateRandomBuffer();

            Vec3f unrotatedSourceBase;
            TFProjectionProperties projectionProperties;
            Vec2ui projectionResolution;
            GeometricSetup* geometricSetup;
            GPUMappedVolume* volume;
            GPUMappedVector* randomSamples;
            int samplesPerBeam;
            GPUMapped<Image>* output;
            GPUMapped<Image>* traversalLength;
        };
    }
}