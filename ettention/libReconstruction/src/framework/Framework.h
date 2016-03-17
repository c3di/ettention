#pragma once
#include "Logger.h"
#include "io/ImageStackDataSourceFactory.h"
#include "setup/parameterset/DebugParameterSet.h"
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
        Framework(Logger& logger);
        ~Framework();

        Logger& getLogger() const;
        DebugParameterSet::ProfilingLevel getProfilingLevel();
        
        VolumeSerializer* getVolumeSerializer();
        ImageStackDatasourceFactory* getImageStackDataSourceFactory();
        ImageStackDatasource* openInputStack();
        ForwardProjectionFactory* getForwardProjectionFactory();
        BackProjectionFactory* getBackProjectionFactory();
        ReconstructionAlgorithm* instantiateReconstructionAlgorithm();
        void parseAndAddParameterSource(ParameterSource* parameterSource);
        void resetParameterSource(ParameterSource* parameterSource);
        AlgebraicParameterSet* getParameterSet();
        PerformanceReport* getPerformanceReport();
        CascadingParameterSource* getParameterSource();
        PluginManager* getPluginManager() const;

        CLAbstractionLayer* getOpenCLStack();
        void initOpenCLStack();
        void destroyOpenCLStack();

        void writeFinalVolume(GPUMappedVolume* volume);
        bool on64bitArchitecture() const;

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
