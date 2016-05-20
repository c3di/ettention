#pragma once

namespace ettention
{
    class Framework;
    class GPUMappedVolume;
    class Visualizer;

    class ReconstructionAlgorithm
    {
    public:
        ReconstructionAlgorithm(Framework* framework);
        virtual ~ReconstructionAlgorithm();

        virtual void run() = 0;
        virtual void runSingleIteration() = 0;
        virtual GPUMappedVolume* getReconstructedVolume() const = 0;
        virtual void exportGeometryTo(Visualizer* visualizer) = 0;

    protected:
        Framework* const framework;
    };
}