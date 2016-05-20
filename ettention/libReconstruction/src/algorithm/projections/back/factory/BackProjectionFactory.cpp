#include "stdafx.h"
#include "BackProjectionFactory.h"
#include "algorithm/projections/back/factory/DefaultBackProjectionCreator.h"

namespace ettention
{
    BackProjectionFactory::InstanceCreator::InstanceCreator()
    {
    }

    BackProjectionFactory::InstanceCreator::~InstanceCreator()
    {
    }

    BackProjectionFactory::BackProjectionFactory()
    {
        this->RegisterCreator(std::unique_ptr<DefaultBackProjectionCreator>(new DefaultBackProjectionCreator()));
    }

    BackProjectionFactory::~BackProjectionFactory()
    {
    }

    void BackProjectionFactory::RegisterCreator(std::unique_ptr<InstanceCreator> creator)
    {
        creators.push_back(std::move(creator));
    }

    BackProjectionKernel* BackProjectionFactory::CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const
    {
        for(auto it = creators.rbegin(); it != creators.rend(); ++it)
        {
            auto kernel = (*it)->CreateKernelInstance(framework, geometricSetup, volume, priorKnowledgeMask);
            if(kernel)
            {
                return kernel;
            }
        }
        return 0;
    }
}