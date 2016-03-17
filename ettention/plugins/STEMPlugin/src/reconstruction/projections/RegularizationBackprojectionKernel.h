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
        class PLUGIN_API RegularizationBackProjectionKernel : public ComputeKernel
        {
        public:
            RegularizationBackProjectionKernel(Framework* framework, const GeometricSetup* geometricSetup, GPUMapped<Image>* residual, GPUMapped<Image>* rayLength, GPUMapped<SubVolume>* subvolume, float lambda, bool useLongObjectCompensation);
            ~RegularizationBackProjectionKernel();

			void setScannerGeometry(STEMScannerGeometry* scannerGeometry);

        protected:
            void prepareKernelArguments();
            void preRun();
            void postRun();

        private:
            GPUMapped<Matrix4x4>* transformMatrix;
            GPUMapped<Image>* rayLength;
            GPUMapped<Image>* residual;
			STEMScannerGeometry* scannerGeometry;
            Vec2ui projectionResolution;
            const GeometricSetup* geometricSetup;
            GPUMapped<SubVolume>* subvolume;
            float lambda;
        };
    }
}