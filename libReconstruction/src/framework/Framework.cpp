#include "stdafx.h"
#include "Framework.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionOperator.h"
#include "framework/plugins/PluginManager.h"
#include "framework/time/PerformanceReport.h"
#include "io/serializer/VolumeSerializer.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/AlgorithmParameterSet.h"
#include "setup/parametersource/CascadingParameterSource.h"

namespace ettention
{
    Framework::Framework(Logger& logger)
        : logger(logger)
        , clStack(0)
        , parameterSourceChanged(false)
    {
        imageStackDataSourceFactory = new ImageStackDatasourceFactory();
        volumeSerializer = new VolumeSerializer();
        forwardProjectionFactory = new ForwardProjectionFactory();
        backProjectionFactory = new BackProjectionFactory();
        pluginManager = new PluginManager(this);
        performanceReport = new PerformanceReport;
        parameterSource = new CascadingParameterSource;
        parameterSet = new AlgebraicParameterSet(parameterSource);
    }

    Framework::~Framework()
    {
        this->destroyOpenCLStack();
        delete parameterSet;
        delete performanceReport;
        delete pluginManager;
        delete backProjectionFactory;
        delete forwardProjectionFactory;
        delete volumeSerializer;
        delete imageStackDataSourceFactory;
        delete parameterSource;
    }

    Logger& Framework::getLogger() const
    {
        return logger;
    }

    ImageStackDatasourceFactory* Framework::getImageStackDataSourceFactory()
    {
        return imageStackDataSourceFactory;
    }

    ImageStackDatasource* Framework::openInputStack()
    {
        return imageStackDataSourceFactory->instantiate(this->parameterSet);
    }

    VolumeSerializer* Framework::getVolumeSerializer()
    {
        return volumeSerializer;
    }

    ForwardProjectionFactory* Framework::getForwardProjectionFactory()
    {
        return forwardProjectionFactory;
    }

    BackProjectionFactory* Framework::getBackProjectionFactory()
    {
        return backProjectionFactory;
    }

    PluginManager* Framework::getPluginManager() const
    {
        return pluginManager;
    }

    CLAbstractionLayer* Framework::getOpenCLStack()
    {
        if(!clStack)
        {
            this->initOpenCLStack();
        }
        return clStack;
    }

    void Framework::initOpenCLStack(const HardwareParameterSet* hardwareParameterSet)
    {
        this->destroyOpenCLStack();
        clStack = new CLAbstractionLayer(this);
    }

    void Framework::destroyOpenCLStack()
    {
        delete clStack;
        clStack = 0;
    }

    AlgebraicParameterSet* Framework::getParameterSet()
    {
        if(parameterSourceChanged)
        {
            parameterSource->parse();
            delete parameterSet;
            parameterSet = new AlgebraicParameterSet(parameterSource);
            parameterSourceChanged = false;
        }
        return parameterSet;
    }

    CascadingParameterSource* Framework::getParameterSource()
    {
        return parameterSource;
    }

    void Framework::parseAndAddParameterSource(ParameterSource* parameterSource)
    {
        if(clStack)
        {
            LOGGER_IMP("Parametersource added after OpenCL initialization! Changed hardware parameters will be ignored!");
        }
        this->parameterSource->parseAndAddSource(parameterSource);
        parameterSourceChanged = true;
    }

    ReconstructionAlgorithm* Framework::instantiateReconstructionAlgorithm()
    {
        auto algorithm = getParameterSet()->get<AlgorithmParameterSet>()->Algorithm();
        std::transform(algorithm.begin(), algorithm.end(), algorithm.begin(), tolower);
        if(algorithm == "sart" || algorithm == "sirt" || algorithm == "blockiterative")
        {
            return new BlockIterativeReconstructionOperator(this);
        }
        else
        {
            return pluginManager->instantiateReconstructionAlgorithm(algorithm, this);
        }
    }

    void Framework::writeFinalVolume(GPUMappedVolume* volume)
    {
        getVolumeSerializer()->write(volume, getParameterSet());
     }

    PerformanceReport* Framework::getPerformanceReport()
    {
        return performanceReport;
    }
}