#pragma once
#include "Logger.h"
#include "io/ImageStackDataSourceFactory.h"
#include "algorithm/projections/back/factory/BackProjectionFactory.h"
#include "algorithm/projections/forward/factory/ForwardProjectionFactory.h"

namespace ettention
{
    class AlgebraicParameterSet;
    class CascadingParameterSource;
    class HardwareParameterSet;
    class PerformanceReport;
    class PluginManager;
    class ReconstructionAlgorithm;
    class VolumeSerializer;
    
    class Framework
    {
    public:
        Framework(Logger& logger, const boost::filesystem::path& pathToPluginDirectory = ".");
        ~Framework();

        Logger& getLogger() const;
        
        VolumeSerializer* getVolumeSerializer();
        ImageStackDatasourceFactory* getImageStackDataSourceFactory();
        ImageStackDatasource* openInputStack();
        ForwardProjectionFactory* getForwardProjectionFactory();
        BackProjectionFactory* getBackProjectionFactory();
        ReconstructionAlgorithm* instantiateReconstructionAlgorithm();
        void parseAndAddParameterSource(ParameterSource* parameterSource);
        AlgebraicParameterSet* getParameterSet();
        PerformanceReport* getPerformanceReport();

        PluginManager* getPluginManager() const;

        CLAbstractionLayer* getOpenCLStack();
        void initOpenCLStack(const HardwareParameterSet* hardwareParameterSet = 0);
        void destroyOpenCLStack();

        void writeFinalVolume(GPUMappedVolume* volume);

    private:
        Logger& logger;
        CascadingParameterSource* parameterSource;
        bool parameterSourceChanged;
        AlgebraicParameterSet* parameterSet;
        CLAbstractionLayer* clStack;
        ImageStackDatasourceFactory* imageStackDataSourceFactory;
        ForwardProjectionFactory* forwardProjectionFactory;
        BackProjectionFactory* backProjectionFactory;
        VolumeSerializer* volumeSerializer;
        PluginManager* pluginManager;
        PerformanceReport* performanceReport;
    };
}
