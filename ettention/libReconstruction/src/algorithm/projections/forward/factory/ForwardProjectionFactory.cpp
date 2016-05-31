#include "stdafx.h"
#include "ForwardProjectionFactory.h"
#include "algorithm/projections/forward/factory/DefaultForwardProjectionCreator.h"
#include "framework/Logger.h"

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

    ForwardProjectionFactory::~ForwardProjectionFactory()
    {
    }

    void ForwardProjectionFactory::RegisterCreator(std::unique_ptr<InstanceCreator> creator)
    {
        creators.push_back(std::move(creator));
    }

    ForwardProjectionKernel* ForwardProjectionFactory::CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const
    {
        for(auto it = creators.rbegin(); it != creators.rend(); ++it)
        {
            auto *kernel = (*it)->CreateKernelInstance(framework, geometricSetup, volume, priorKnowledgeMask);
            if(kernel)
            {
                LOGGER_IMP((boost::format("ForwardProjectionFactory chosen as responsible %1%.") % kernel->getKernelName()).str());
                return kernel;
            }
        }
        return 0;
    }

}