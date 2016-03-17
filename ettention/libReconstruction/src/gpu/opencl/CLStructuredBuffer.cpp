#include "stdafx.h"
#include "CLStructuredBuffer.h"
#include "CLImageProperties.h"
#include "CLKernel.h"
#include "CLMemBuffer.h"

namespace ettention
{
    CLStructuredBuffer::CLStructuredBuffer(CLAbstractionLayer* clal, const CLImageProperties& imgProps)
    {
        dataMemObject = new CLMemBuffer(clal, imgProps.getImageByteWidth());
        properties.resolution = imgProps.getResolution();
        properties.addressingMode = imgProps.getAddressingMode();
        propertiesMemObject = new CLMemBuffer(clal, sizeof(BufferImageProperties), &properties);
    }

    CLStructuredBuffer::~CLStructuredBuffer()
    {
        delete propertiesMemObject;
        delete dataMemObject;
    }

    void CLStructuredBuffer::setAsArgument(CLKernel* kernel, int argNumber)
    {
        kernel->setArgument(argNumber, dataMemObject);
        kernel->setArgument(argNumber + 1, propertiesMemObject);
    }

    CLMemObject* CLStructuredBuffer::getDataMemObject() const
    {
        return dataMemObject;
    }
}