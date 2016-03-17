#pragma once
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/GPUMapped.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/image/Image.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "reconstruction/projections/STEMForwardProjection.h"
#include "setup/TF_ART_ParameterSet.h"

namespace ettention
{
    namespace stem
    {
		class STEMScannerGeometry;

        class PLUGIN_API STEMForwardProjectionOperator
        {
        public:
            STEMForwardProjectionOperator(Framework* framework, const Vec2ui& projectionResolution, GeometricSetup* geometricSetup, const Vec3f& unrotatedSourceBase, GPUMappedVolume* volume, unsigned int samplesPerBeam);
            virtual ~STEMForwardProjectionOperator();

            virtual void run();

            GPUMapped<Image>* getVirtualProjection();
            GPUMapped<Image>* getRayLength();
			void setOutput(GPUMapped<Image>* virtualProjection, GPUMapped<Image>* traversalLength);


			void setScannerGeometry(STEMScannerGeometry* geometry);

        protected:
            Vec2ui projectionResolution;
            GeometricSetup* geometricSetup;
            GPUMappedVolume* volume;
            TF_ART_ParameterSet* parameterSet;
            unsigned int samplesPerBeam;

            STEMForwardProjectionKernel* forwardKernel;
        };

    }
}