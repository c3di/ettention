#pragma once
#include "algorithm/reconstruction/BlockIterative/BlockIterativeProjectionAccess.h"
#include "gpu/GPUMapped.h"
#include "model/image/Image.h"

namespace ettention
{
    class BlockIterativeState
    {
    public:
        BlockIterativeState(CLAbstractionLayer* al, BlockIterativeProjectionAccess* projectionAccess);
        ~BlockIterativeState();

        void setCurrentBlockIndex(unsigned int blockIndex);
        void setCurrentProjectionIndex(unsigned int projectionIndex);
        GPUMapped<Image>* getVirtualProjection() const;
        GPUMapped<Image>* getTraversalLength() const;
        GPUMapped<Image>* getResidual() const;
        GPUMapped<Image>* getCurrentRealProjection() const;
        unsigned int getCurrentBlockIndex() const;
        unsigned int getCurrentProjectionIndex() const;
        unsigned int getCurrentPhysicalProjectionIndex() const;
        unsigned int getCurrentBlockSize() const;
        Image* getCurrentRealProjectionImage() const;
        ScannerGeometry getCurrentScannerGeometry() const;

    private:
        CLAbstractionLayer* al;
        BlockIterativeProjectionAccess* projectionAccess;
        unsigned int currentBlockIndex;
        unsigned int currentProjectionIndex;
        GPUMapped<Image>* currentRealProjection;
        std::vector<GPUMapped<Image>*> virtualProjections;
        std::vector<GPUMapped<Image>*> traversalLengths;
        std::vector<GPUMapped<Image>*> residuals;
    };
}