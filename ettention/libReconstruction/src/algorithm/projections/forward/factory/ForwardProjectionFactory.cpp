#include "stdafx.h"
#include "ForwardProjectionFactory.h"
#include "algorithm/projections/forward/factory/DefaultForwardProjectionCreator.h"

namespace ettention
{
    ForwardProjectionFactory::InstanceCreator::InstanceCreator()
    {
    }

    ForwardProjectionFactory::InstanceCreator::~InstanceCreator()
    {
    }

    ForwardProjectionFactory::ForwardProjectionFactory()
    {
        this->RegisterCreator(std::unique_ptr<DefaultForwardProjectionCreator>(new DefaultForwardProjectionCreator));
    }

    void ForwardProjectionFactory::RegisterCreator(std::unique_ptr<InstanceCreator> creator)
    {
        creators.push_back(std::move(creator));
    }

    ForwardProjectionKernel* ForwardProjectionFactory::CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const
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