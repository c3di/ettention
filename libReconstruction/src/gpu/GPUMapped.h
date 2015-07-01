#pragma once
#include "gpu/VectorAdaptor.h"
#include "model/volume/VoxelType.h"
#include "algorithm/projections/back/VoxelSample.h"

/* An GPUMapped represents an objects that can be accessed either on the CPU or on the GPU. It supports to transfer data 
   from CPU memory to GPU memory and back.
   Usage: initialize the GPUMapped<Object> by either passing an object, for example an image (CPU), or a Buffer (GPU) or both. GPUMapped<Object> will 
          create the missing representation and ensure that it is released when the GPUMapped<Object> is destroyed. 
   Before reading any representation, call ensureIsUpToDateOnGPU or ensureIsUpToDateOnCPU respectively.
   After changing the values, call wasChangedOnCPU or wasChangedOnGPU to indicate the change. 

   In order to be used with the template, the class T needs the following members:

   typedef typename T::ResolutionType;
   Constructor (ResolutionType);
   ResolutionType Resolution();
   float* DataPtr();
   std::size_t getByteWidth();
 */

namespace ettention
{
    class CLAbstractionLayer;
    class CLMemBuffer;
    class CLMemoryStructure;

    template<class T>
    class GPUMapped
    {
    public:
        typedef typename T::ResolutionType ResolutionType;

        GPUMapped(CLAbstractionLayer* clStack, ResolutionType resolution, bool useImage = false, VoxelType voxelType = VoxelType::FLOAT_32);
        GPUMapped(CLAbstractionLayer* clStack, T* objectOnCPU, bool useImage = false, VoxelType voxelType = VoxelType::FLOAT_32);
        GPUMapped(T* image, CLMemBuffer* buffer, bool useImage = false, VoxelType voxelType = VoxelType::FLOAT_32);

        virtual ~GPUMapped();

        void markAsChangedOnCPU();
        void markAsChangedOnGPU();

        void ensureImageIsUpToDate();
        void ensureIsUpToDateOnGPU();
        void ensureIsUpToDateOnCPU();

        bool hasChangedOnCPU() const;
        bool hasChangedOnGPU() const;

        T* getObjectOnCPU();
        CLMemBuffer* getBufferOnGPU();
        bool doesUseImage();
        CLMemoryStructure* getImageOnGPU();

        ResolutionType getResolution();

        void setObjectOnCPU(T* newObjectOnCPU);
        void takeOwnershipOfObjectOnCPU();

        void reallocateGPUBuffer();

    protected:
        void allocateMemoryStructure();
        void returnMemoryStructure();
        void reallocateMemoryStructure();

    protected:
        CLAbstractionLayer* clStack;

        T* objectOnCPU;
        CLMemBuffer* bufferOnGPU;

        bool isImageValid;
        bool isValidOnGPU;
        bool isValidOnCPU;

        bool ownsCPUObject;
        bool ownsGPUBuffer;

        bool useImage;
        CLMemoryStructure* memoryStructure;
        VoxelType voxelType;
        ResolutionType resolution;
    };

    typedef GPUMapped<VectorAdaptor<float>> GPUMappedVector;
    typedef GPUMapped<VectorAdaptor<int>> GPUMappedIntVector;
    typedef GPUMapped<VectorAdaptor<unsigned int>> GPUMappedUIntVector;
    typedef GPUMapped<VectorAdaptor<unsigned short>> GPUMappedUShortVector;
    typedef GPUMapped<VectorAdaptor<VoxelSample>> GPUMappedSampleVector;
}