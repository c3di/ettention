#include "stdafx.h"

#include "gpu/imagerepresentation/GPURepresentationOfImage.h"
                    
namespace ettention
{
    GPURepresentationOfImage::GPURepresentationOfImage(Vec3ui resolution)
        :resolution(resolution)
    {

    }

    GPURepresentationOfImage::~GPURepresentationOfImage()
    {

    }

    Vec3ui GPURepresentationOfImage::getResolution()
    {
        return resolution;
    }
}