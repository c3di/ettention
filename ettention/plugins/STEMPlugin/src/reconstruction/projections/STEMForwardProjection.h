#pragma once

#include "algorithm/projections/forward/ForwardProjectionKernel.h"
#include "gpu/GPUMapped.h"
#include "gpu/ComputeKernel.h"
#include "model/volume/Volume.h"
#include "model/volume/SubVolume.h"

namespace ettention
{
    class GPUMappedVolume;
    class GeometricSetup;

    namespace stem
    {
		class STEMScannerGeometry;

		class PLUGIN_API STEMForwardProjectionKernel : public ForwardProjectionKernel
        {
        public:
            STEMForwardProjectionKernel(Framework* framework, 
										const Vec2ui& projectionResolution, 
										GeometricSetup* geometricSetup, 
										const Vec3f& unrotatedSourceBase, 
										GPUMappedVolume* volume,
										GPUMappedVolume* priorKnowledgeMask,
										int samplesPerBeam);
            virtual ~STEMForwardProjectionKernel();

			void setScannerGeometry(STEMScannerGeometry* geometry);

        protected:
            void prepareKernelArguments() override;

        private:
            void allocateRandomBuffer();

            Vec3f unrotatedSourceBase;
			STEMScannerGeometry* geometry;
            GPUMappedVector* randomSamples;
        };
    }
}