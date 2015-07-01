#pragma once
#include "algorithm/projections/back/BackProjectionKernel.h"

namespace ettention
{
    namespace contour
    {
        class PLUGIN_API ContourBackProjectionKernel : public BackProjectionKernel
        {
        public:
            ContourBackProjectionKernel(Framework* framework,
                                        const GeometricSetup* geometricSetup,
                                        GPUMappedVolume* volume,
                                        GPUMappedVolume* priorKnowledgeMask,
                                        float lambda,
                                        bool useLongObjectCompensation,
                                        bool useProjectionsAsImages,
                                        const std::string& additionalDefines = "");
            ~ContourBackProjectionKernel();

        protected:
            void prepareKernelArguments() override;

        private:
            GPUMapped<Matrix4x4>* transform;
        };
    }
}