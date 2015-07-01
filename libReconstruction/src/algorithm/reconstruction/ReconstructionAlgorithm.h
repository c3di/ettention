#pragma once
#include "framework/Framework.h"

namespace ettention
{
    class ReconstructionAlgorithm
    {
    public:
        ReconstructionAlgorithm(Framework* framework);
        virtual ~ReconstructionAlgorithm();

        virtual void run() = 0;
        virtual GPUMappedVolume* getReconstructedVolume() const = 0;        

    protected:
        Framework* const framework;
    };
}