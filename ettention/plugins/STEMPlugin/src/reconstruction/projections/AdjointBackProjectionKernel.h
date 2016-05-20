#pragma once

#include "framework/math/Matrix4x4.h"
#include "gpu/ComputeKernel.h"
#include "model/STEMScannerGeometry.h"

namespace ettention
{
    class GeometricSetup;
    class Image;
    class SubVolume;
    template<typename _T> class GPUMapped;

    namespace stem 
    {
        class PLUGIN_API AdjointBackProjectionKernel : public ComputeKernel
        {
        public:
            AdjointBackProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMapped<Image>* preFilteredResidual, GPUMapped<Image>* rayLength, GPUMapped<SubVolume>* subvolume, float lambda, bool useLongObjectCompensation);
            ~AdjointBackProjectionKernel();

			void setScannerGeometry(STEMScannerGeometry* geometry);

        protected:
            void prepareKernelArguments() override;
            void preRun() override;
            void postRun() override;

        private:
            GPUMapped<Matrix4x4>* transformMatrix;
            GPUMapped<Image>* rayLength;
            GPUMapped<Image>* preFilteredResidual;
			STEMScannerGeometry* scannerGeometry;
            Vec2ui projectionResolution;
            const GeometricSetup* geometricSetup;
            GPUMapped<SubVolume>* subvolume;
            float lambda;
        };
    }
}