#include "stdafx.h"
#include "GPUMapped.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "gpu/opencl/CLImageProperties.h"
#include "gpu/opencl/CLMemoryStructure.h"
#include "model/image/Image.h"
#include "model/volume/SubVolume.h"
#include "framework/math/Matrix4x4.h"
#include "framework/time/Timer.h"

namespace ettention 
{
    template<class T>
    GPUMapped<T>::GPUMapped(CLAbstractionLayer* clStack, ResolutionType resolution, bool useImage, Voxel::DataType voxelType)
        : clStack(clStack)
        , ownsCPUObject(true)
        , ownsGPUBuffer(true)
        , isValidOnGPU(true)
        , isValidOnCPU(true)
        , useImage(useImage)
        , voxelType(voxelType)
    {
        objectOnCPU = new T(resolution);
        bufferOnGPU = new CLMemBuffer(clStack, objectOnCPU->getByteWidth());
        if(useImage)
        {
            allocateMemoryStructure();
        }
    }

    template<class T>
    GPUMapped<T>::GPUMapped(CLAbstractionLayer* clStack, T* objectOnCPU, bool useImage, Voxel::DataType voxelType)
        : clStack(clStack)
        , objectOnCPU(objectOnCPU)
        , ownsCPUObject(false)
        , useImage(useImage)
        , voxelType(voxelType)
    {
        bufferOnGPU = new CLMemBuffer(clStack, objectOnCPU->getByteWidth());
        if(useImage)
            allocateMemoryStructure();

        ownsGPUBuffer = true;

        isValidOnGPU = false;
        isValidOnCPU = true;
    }

    template<class T>
    GPUMapped<T>::GPUMapped(T* objectOnCPU, CLMemBuffer* buffer, bool useImage, Voxel::DataType voxelType)
        : clStack(NULL)
        , objectOnCPU(objectOnCPU)
        , bufferOnGPU(buffer)
        , ownsCPUObject(false)
        , ownsGPUBuffer(false)
        , useImage(useImage)
        , voxelType(voxelType)
    {
        if(useImage)
            allocateMemoryStructure();

        isValidOnGPU = true;
        isValidOnCPU = true;
    }

    template<class T>
    GPUMapped<T>::~GPUMapped()
    {
        if(useImage)
            returnMemoryStructure();
        if(ownsGPUBuffer)
            delete bufferOnGPU;
        if(ownsCPUObject)
        {
            delete objectOnCPU;
        }
    }

    template<class T>
    void GPUMapped<T>::markAsChangedOnCPU()
    {
        isValidOnGPU = false;
        isValidOnCPU = true;
        if(bufferOnGPU->getByteWidth() < objectOnCPU->getByteWidth())
        {
            if(!ownsGPUBuffer)
            {
                ETTENTION_THROW_EXCEPTION("GPUMapped object of size " << objectOnCPU->getByteWidth() << "B does not fit into GPU buffer of size " << bufferOnGPU->getByteWidth() << "B!");
            }
            else
            {
                delete bufferOnGPU;
                bufferOnGPU = new CLMemBuffer(clStack, objectOnCPU->getByteWidth());
            }
        }
    }

    template<class T>
    void GPUMapped<T>::markAsChangedOnGPU()
    {
        isValidOnCPU = false;
        isValidOnGPU = true;
        isImageValid = false;
    }

    template<class T>
    void GPUMapped<T>::ensureImageIsUpToDate()
    {
        ensureIsUpToDateOnGPU();
        if(useImage && !isImageValid)
        {
            bufferOnGPU->copyTo(memoryStructure->getDataMemObject());
            isImageValid = true;
        }
    }

    template<class T>
    void GPUMapped<T>::ensureIsUpToDateOnGPU()
    {
        if(isValidOnGPU)
            return;
        bufferOnGPU->transferFrom(objectOnCPU->getData(), objectOnCPU->getByteWidth());      
        isValidOnCPU = true;
        isValidOnGPU = true;
        isImageValid = false;
    }

    template<class T>
    void GPUMapped<T>::ensureIsUpToDateOnCPU()
    {
        if(isValidOnCPU)        
            return;
        bufferOnGPU->transferTo(objectOnCPU->getData(), objectOnCPU->getByteWidth());
        isValidOnCPU = true;
        isValidOnGPU = true;
    }

    template<class T>
    bool GPUMapped<T>::hasChangedOnCPU() const
    {
        return !isValidOnGPU;
    }

    template<class T>
    bool GPUMapped<T>::hasChangedOnGPU() const
    {
        return !isValidOnCPU;
    }

    template<class T>
    T* GPUMapped<T>::getObjectOnCPU()
    {
        return objectOnCPU;
    }

    template<class T>
    CLMemBuffer* GPUMapped<T>::getBufferOnGPU()
    {
        return bufferOnGPU;
    }

    template<class T>
    bool GPUMapped<T>::doesUseImage()
    {
        return useImage;
    }

    template<class T>
    CLMemoryStructure* GPUMapped<T>::getImageOnGPU()
    {
        if(!useImage)
            throw Exception("cannot access GPU-Image of object was mapped to buffer only. Use the useImage-flag in the constructor to map to an GPU-Image.");
        return memoryStructure;
    }

    template<class T>
    void GPUMapped<T>::setObjectOnCPU(T* newObjectOnCPU)
    {
        if(objectOnCPU != newObjectOnCPU)
        {
            bool resolutionChanged = objectOnCPU->getResolution() != newObjectOnCPU->getResolution();
            if(ownsCPUObject)
            {
                delete objectOnCPU;
            }
            objectOnCPU = newObjectOnCPU;
            ownsCPUObject = false;
            markAsChangedOnCPU();
            if(resolutionChanged)
            {
                reallocateGPUBuffer();
            }
        }
    }

    template<class T>
    void GPUMapped<T>::takeOwnershipOfObjectOnCPU()
    {
        ownsCPUObject = true;
    }

    template<class T>
    void GPUMapped<T>::reallocateGPUBuffer()
    {
        if(!ownsGPUBuffer)
            throw Exception("GPUMapped<> cannot reallocate GPU Buffer unless it owns it. If you provide the Buffer externally, resolution must stay constant.");

        delete bufferOnGPU;
        bufferOnGPU = new CLMemBuffer(clStack, objectOnCPU->getByteWidth());
        isValidOnGPU = false;
        if(useImage)
        {
            reallocateMemoryStructure();
        }
    }

    template<class T>
    typename T::ResolutionType GPUMapped<T>::getResolution()
    {
        return objectOnCPU->getResolution();
    }    

    template<class T>
    void GPUMapped<T>::allocateMemoryStructure()
    {
        CLImageProperties imgProps;
        imgProps.setFilterMode(CL_FILTER_NEAREST);
        imgProps.setNormalizedCoordinates(false);
        imgProps.setAddressingMode(CL_ADDRESS_CLAMP);
        imgProps.setChannelType(Voxel::getCorrespondingCLTextureChannelType(voxelType));
        imgProps.setResolution(objectOnCPU->getResolution());
        memoryStructure = CLMemoryStructure::create(clStack, imgProps);
    }

    template<class T>
    void GPUMapped<T>::returnMemoryStructure()
    {
        delete memoryStructure;
    }

    template<class T>
    void GPUMapped<T>::reallocateMemoryStructure()
    {
        returnMemoryStructure();
        allocateMemoryStructure();
    }

    template class GPUMapped<Image>;
    template class GPUMapped<SubVolume>;
    template class GPUMapped<VectorAdaptor<int>>;
    template class GPUMapped<VectorAdaptor<unsigned int>>;
    template class GPUMapped<VectorAdaptor<unsigned short>>;
    template class GPUMapped<VectorAdaptor<float>>;
    template class GPUMapped<VectorAdaptor<VoxelSample>>;
    template class GPUMapped<Matrix4x4>;
}
