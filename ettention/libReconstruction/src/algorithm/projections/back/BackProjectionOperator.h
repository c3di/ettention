#pragma once

namespace ettention
{
    class BackProjectionKernel;
    class Framework;
    class GeometricSetup;
    class GPUMappedVolume;
    class Image;
    class Voxelizer;
    template<typename _T> class GPUMapped;

    class BackProjectionOperator
    {
    public:
        static const unsigned int ALL_SUB_VOLUMES = (unsigned int)-1;

        BackProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask);
        virtual ~BackProjectionOperator();

        void execute(unsigned int subVolumeIndex = ALL_SUB_VOLUMES);
        GPUMappedVolume* getRawVolume();
        BackProjectionKernel* getKernel();
        Voxelizer* getVoxelizer();
        void setInput(GPUMapped<Image>* residual, GPUMapped<Image>* traversalLength, unsigned int projectionBlockSize);

    private:
        Framework* framework;
        BackProjectionKernel* kernel;
        GeometricSetup* geometricSetup;
        GPUMappedVolume* volume;
        GPUMappedVolume* priorKnowledgeMask;
        Voxelizer* voxelizer;

        void executeForSingleSubVolume(unsigned int subVolumeIndex);
    };
}

