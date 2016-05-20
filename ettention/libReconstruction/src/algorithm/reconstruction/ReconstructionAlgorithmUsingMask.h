#pragma once

#include "ReconstructionAlgorithm.h"

namespace ettention
{
    class Framework;
    class GPUMappedVolume;

    class ReconstructionAlgorithmUsingMask : public ReconstructionAlgorithm
    {
    public:
        ReconstructionAlgorithmUsingMask(Framework* framework);
        virtual ~ReconstructionAlgorithmUsingMask();

        virtual void setMaskVolume(GPUMappedVolume* volume) = 0;
        virtual GPUMappedVolume* getMaskVolume() const = 0;
    };
}