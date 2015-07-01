#include "stdafx.h"
#include "CLMemImage.h"
#include "CLAbstractionLayer.h"
#include "CLMemBuffer.h"
#include "framework/NumericalAlgorithms.h"

namespace ettention
{
    CLMemImage::CLMemImage(CLAbstractionLayer* clal, const CLImageProperties& imgProps, const void* initialData)
        : CLMemObject(clal, imgProps.getImageByteWidth())
        , imgProps(imgProps)
    {
        cl_image_format format;
        format.image_channel_data_type = imgProps.getChannelType();
        format.image_channel_order = imgProps.getChannelOrder();
        cl_int err = 0;
        cl_mem clMem = 0;
//         cl_image_desc desc;
//         std::memset(&desc, 0, sizeof(cl_image_desc));
//         desc.image_width = imgProps.getResolution().x;
//         desc.image_height = imgProps.getResolution().y;
//         desc.image_depth = imgProps.getResolution().z;
//         desc.image_array_size = 1;
        if(imgProps.getResolution().z == 1)
        {
//             desc.image_type = CL_MEM_OBJECT_IMAGE2D;
            clMem = clCreateImage2D(clal->getContext(), 0, &format, imgProps.getResolution().x, imgProps.getResolution().y, 0, 0, &err);
            CL_ASSERT(err);
        }
        else
        {
//             desc.image_type = CL_MEM_OBJECT_IMAGE3D;
            clMem = clCreateImage3D(clal->getContext(), 0, &format, imgProps.getResolution().x, imgProps.getResolution().y, imgProps.getResolution().z, 0, 0, 0, &err);
            CL_ASSERT(err);
        }
//         clMem = clCreateImage(clal->getContext(), 0, &format, &desc, 0, &err);
//         CL_ASSERT(err);
        setCLMem(clMem);
        if(initialData)
        {
            transferFrom(initialData);
        }
        else
        {
            clear();
        }
    }

    CLMemImage::~CLMemImage()
    {
    }

    void CLMemImage::transferFrom(const void* source)
    {
        size_t origin[3] = { 0, 0, 0, };
        size_t region[3] = { imgProps.getResolution().x, imgProps.getResolution().y, imgProps.getResolution().z, };
        cl_event evt = 0;
        CL_ASSERT(clEnqueueWriteImage(clal->getCommandQueue(), getCLMem(), CL_TRUE, origin, region, 0, 0, source, 0, 0, &evt));
        clal->profileAndReleaseEvent("Transfer from CPU to CLMemImage (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
    }

    void CLMemImage::transferTo(void* dest)
    {
        size_t origin[3] = { 0, 0, 0, };
        size_t region[3] = { imgProps.getResolution().x, imgProps.getResolution().y, imgProps.getResolution().z, };
        cl_event evt = 0;
        CL_ASSERT(clEnqueueReadImage(clal->getCommandQueue(), getCLMem(), CL_TRUE, origin, region, 0, 0, dest, 0, 0, &evt));
        clal->profileAndReleaseEvent("Transfer from CLMemImage to CPU (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
    }

    void CLMemImage::copyTo(CLMemObject* dest)
    {
        if(byteWidth != dest->getByteWidth())
        {
            throw GPUException("Bytewidths of CLMemImage and CLMemObject don't match!");
        }
        size_t origin[3] = { 0, 0, 0, };
        size_t region[3] = { imgProps.getResolution().x, imgProps.getResolution().y, imgProps.getResolution().z, };
        cl_event evt = 0;
        if(dynamic_cast<CLMemImage*>(dest))
        {
            if(dynamic_cast<CLMemImage*>(dest)->imgProps.getResolution() != imgProps.getResolution())
            {
                throw GPUException("Image resolutions do not match!");
            }
            CL_ASSERT(clEnqueueCopyImage(clal->getCommandQueue(), getCLMem(), dest->getCLMem(), origin, origin, region, 0, 0, &evt));
            clal->profileAndReleaseEvent("Copy from CLMemImage to CLMemImage (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
        }
        else if(dynamic_cast<CLMemBuffer*>(dest))
        {
            CL_ASSERT(clEnqueueCopyImageToBuffer(clal->getCommandQueue(), getCLMem(), dest->getCLMem(), origin, region, 0, 0, 0, &evt));
            clal->profileAndReleaseEvent("Copy from CLMemImage to CLMemBuffer (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
        }
        else
        {
            throw GPUException(std::string("Copy from CLMemImage to ") + typeid(dest).name() + " not supported!");
        }
    }

    const CLImageProperties& CLMemImage::getProperties() const
    {
        return imgProps;
    }
}