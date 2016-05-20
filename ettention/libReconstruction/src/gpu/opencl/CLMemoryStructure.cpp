#include "stdafx.h"
#include "CLMemoryStructure.h"
#include "CLAbstractionLayer.h"
#include "CLImage.h"
#include "CLStructuredBuffer.h"

namespace ettention
{
    CLMemoryStructure::CLMemoryStructure()
    {
    }

    CLMemoryStructure::~CLMemoryStructure()
    {
    }

    CLMemoryStructure* CLMemoryStructure::create(CLAbstractionLayer* clal, const CLImageProperties& imgProps)
    {
        return clal->hasImageSupport() ? (CLMemoryStructure*)new CLImage(clal, imgProps) : (CLMemoryStructure*)new CLStructuredBuffer(clal, imgProps);
    }
}