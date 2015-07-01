#pragma once

#include "framework/Framework.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "io/ImageStackDataSourceFactory.h"

#include "algorithm/reconstruction/ReconstructionAlgorithm.h"
#include "algorithm/projections/forward/ForwardProjectionOperator.h"
#include "model/projectionset/ProjectionSet.h"

namespace ettention
{
    class VolumeSerializer;

    class Plugin
    {
    public:
        Plugin();
        virtual ~Plugin();;

        virtual std::string getName() = 0;

        Framework* getFramework();
        void setFramework(Framework* val);

        virtual ReconstructionAlgorithm* instantiateReconstructionAlgorithm(const std::string& identifier, Framework* framework);

        virtual void registerImageFileFormat(ImageStackDatasourceFactory* factory);
        virtual void registerVolumeFileFormat(VolumeSerializer* factory);

        virtual ProjectionSet* instantiateProjectionIterator(const std::string& identifier, ImageStackDatasource* source);

        virtual void registerForwardProjectionCreator(ForwardProjectionFactory* factory);
        virtual void registerBackProjectionCreator(BackProjectionFactory* factory);

    protected:
        Framework* framework;

    };
}