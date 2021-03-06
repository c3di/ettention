#include "stdafx.h"
#include "Framework.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeReconstructionOperator.h"
#include "algorithm/reconstruction/discrete/DiscreteReconstructionAlgorithm.h"
#include "framework/geometry/Visualizer.h"
#include "framework/plugins/PluginManager.h"
#include "framework/time/PerformanceReport.h"
#include "io/serializer/VolumeSerializer.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/parameterset/DebugParameterSet.h"
#include "setup/parametersource/CascadingParameterSource.h"

namespace ettention
{
    Framework::Framework(Logger& logger)
        : logger(logger)
        , clStack(nullptr)
        , parameterSourceChanged(false)
        , performanceReport(nullptr)
    {
        imageStackDataSourceFactory = new ImageStackDatasourceFactory();
        volumeSerializer = new VolumeSerializer();
        forwardProjectionFactory = new ForwardProjectionFactory();
        backProjectionFactory = new BackProjectionFactory();
        pluginManager = new PluginManager(this);
        parameterSource = new CascadingParameterSource;
        parameterSet = new AlgebraicParameterSet(parameterSource);
    }

    Framework::~Framework()
    {
        destroyOpenCLStack();
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

    DebugParameterSet::ProfilingLevel Framework::getProfilingLevel()
    {
        return getParameterSet()->get<DebugParameterSet>()->getProfilingLevel();
    }

    ImageStackDatasourceFactory* Framework::getImageStackDataSourceFactory()
    {
        return imageStackDataSourceFactory;
    }

    ImageStackDatasource* Framework::openInputStack()
    {
        return imageStackDataSourceFactory->instantiate(parameterSet);
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
            initOpenCLStack();
        }
        return clStack;
    }

    void Framework::initOpenCLStack()
    {
        destroyOpenCLStack();
        clStack = new CLAbstractionLayer(this);
    }

    void Framework::destroyOpenCLStack()
    {
        delete clStack;
        clStack = nullptr;
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

    void Framework::resetParameterSource(ParameterSource* parameterSource)
    {
        delete this->parameterSource;
        delete this->parameterSet;
        this->parameterSource = new CascadingParameterSource;
        this->parameterSet = new AlgebraicParameterSet(this->parameterSource);
        parseAndAddParameterSource(parameterSource);
    }

    ReconstructionAlgorithm* Framework::instantiateReconstructionAlgorithm()
    {
        getParameterSet()->get<OutputParameterSet>()->testValidityOfOutputOptions(getVolumeSerializer());

        auto algorithm = getParameterSet()->get<AlgorithmParameterSet>()->getAlgorithm();
        std::transform(algorithm.begin(), algorithm.end(), algorithm.begin(), tolower);
        
        ReconstructionAlgorithm* reconstructionAlgorithm = nullptr;
        if(algorithm == "sart" || algorithm == "sirt" || algorithm == "blockiterative")
        {
            reconstructionAlgorithm = new BlockIterativeReconstructionOperator(this);
        } else {
            reconstructionAlgorithm = pluginManager->instantiateReconstructionAlgorithm(algorithm, this);
        }

        auto isDiscrete = getParameterSet()->get<AlgorithmParameterSet>()->isReconstructionDiscrete();
        if( isDiscrete )
        {
            ReconstructionAlgorithmUsingMask *maskedReconstructionAlgorithm = dynamic_cast<ReconstructionAlgorithmUsingMask *>(reconstructionAlgorithm);
            if( !maskedReconstructionAlgorithm )
            {
                throw Exception((boost::format("Discrete reconstruction cannot be done using %1% algorithm, because it does NOT support prior knowledge mask.") % algorithm).str());
            }
            auto discreteReconstructionAlgorithm = new DiscreteReconstructionAlgorithm(this, maskedReconstructionAlgorithm);
            discreteReconstructionAlgorithm->takeOwnershipOfMethod();
            reconstructionAlgorithm = discreteReconstructionAlgorithm;
        }

        auto meshExportPath = getParameterSet()->get<DebugParameterSet>()->getMeshExportPath();
        if(meshExportPath)
        {
            Visualizer* visualizer = new Visualizer(meshExportPath->string(), "ettention");
            reconstructionAlgorithm->exportGeometryTo(visualizer);
            visualizer->writeMesh();
            delete visualizer;
        }

        return reconstructionAlgorithm;
    }

    void Framework::writeFinalVolume(GPUMappedVolume* volume)
    {
        getVolumeSerializer()->write(volume, getParameterSet());
    }

    PerformanceReport* Framework::getPerformanceReport()
    {
        if(!performanceReport)
        {
            auto debugParameterSet = getParameterSet()->get<DebugParameterSet>();
            if( debugParameterSet->getProfilingLevel() >= DebugParameterSet::ProfilingLevel::DETAIL )
            {
                performanceReport = new PerformanceReport(debugParameterSet->getPerformanceReportFile());
            }
        }
        return performanceReport;
    }

    bool Framework::on64bitArchitecture() const
    {
        return sizeof(void*) == 8;
    }

	void Framework::printCLDeviceList(bool printExtensions)
	{
		CLContextCreator creator;
		auto devices = creator.GatherDeviceInfos();
		std::cout << std::endl;
		std::cout << " OpenCL devices on this machine" << std::endl;
		std::cout << "--------------------------------" << std::endl;
		std::cout << " ID | Type | Detail" << std::endl;
		std::cout << "----|------|";
		std::stringstream table;
		unsigned int maxDetailLength = 0;
		for (unsigned int i = 0; i < devices.size(); ++i)
		{
			std::string detail = devices[i].name + ", " + devices[i].version;
			table << " " << std::setfill(' ') << std::setw(2) << i << " | " << devices[i].type << "  | " << detail << std::endl;
			if (printExtensions)
			{
				for (auto it = devices[i].extensions.begin(); it != devices[i].extensions.end(); ++it)
				{
					if (!(*it).empty())
					{
						table << "    |      |  " << *it << std::endl;
						maxDetailLength = std::max(maxDetailLength, (unsigned int)(*it).length());
					}
				}
			}
			table << "    |      |" << std::endl;
			maxDetailLength = std::max(maxDetailLength, (unsigned int)detail.length());
		}
		for (unsigned int i = 0; i < maxDetailLength + 2; ++i)
		{
			std::cout << "-";
		}
		std::cout << std::endl << "    |      |" << std::endl << table.str();
		std::cout << "----|------|";
		for (unsigned int i = 0; i < maxDetailLength + 2; ++i)
		{
			std::cout << "-";
		}
		std::cout << std::endl;
	}

}