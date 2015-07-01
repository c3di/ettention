#include "stdafx.h"
#include "TF_ART.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "io/serializer/VolumeSerializer.h"
#include "reconstruction/operators/Adjoint_BackProjection_Operator.h"
#include "reconstruction/operators/Regularization_BackProjection_Operator.h"
#include "setup/ParameterSet/AlgorithmParameterSet.h"
#include "setup/ParameterSet/DebugParameterSet.h"
#include "setup/ParameterSet/ForwardProjectionParameterSet.h"
#include "STEMPlugin.h"

namespace ettention
{
    namespace stem
    {
        TF_ART::TF_ART(Framework* framework)
            : ReconstructionAlgorithm(framework)
            , clContext(framework->getOpenCLStack())
            , parameterSet(framework->getParameterSet()->get<TF_ART_ParameterSet>())
        {
            ownsProjectionDataSource = true;
            projectionDataSource = dynamic_cast<TF_Datasource*>(framework->getImageStackDataSourceFactory()->instantiate(framework->getParameterSet()));
            if(!projectionDataSource)
            {
                throw Exception("TF_ART does only work with TF_Datasource!");
            }
            projectionResolution = projectionDataSource->getResolution();

            initializeProjectionSet();
            allocateDataBuffer();
            initializeMicroscopeGeometry();
            initializeComputeKernel();
        }

        TF_ART::TF_ART(TF_Datasource* projectionDataSource, Framework* framework)
            : ReconstructionAlgorithm(framework)
            , clContext(framework->getOpenCLStack())
            , parameterSet(framework->getParameterSet()->get<TF_ART_ParameterSet>())
            , projectionDataSource(projectionDataSource)
        {
            ownsProjectionDataSource = false;
            projectionResolution = projectionDataSource->getResolution();

            initializeProjectionSet();
            allocateDataBuffer();
            initializeMicroscopeGeometry();
            initializeComputeKernel();
        }

        TF_ART::~TF_ART()
        {
            freeDataBuffer();
            deleteComputeKernel();
            delete measuredProjection;
            delete forwardProjectionOperator;
            delete backprojectionOperator;
            if(ownsProjectionDataSource)
            {
                delete projectionDataSource;
            }
            delete geometricSetup;
        }

        void TF_ART::run()
        {
            if(parameterSet->shouldUseAutofocus())
                autofocus = new AutomaticFocusComputation(parameterSet, baseScannerGeometry, &projectionSet, projectionDataSource, ownedVolume);

            auto numberOfIterations = framework->getParameterSet()->get<AlgorithmParameterSet>()->NumberOfIterations();
            for(currentIteration = 1; currentIteration <= numberOfIterations; currentIteration++)
            {
                handleOneIteration();
            }
            writeFinalVolume();
        }

        GPUMappedVolume* TF_ART::getReconstructedVolume() const
        {
            return volume;
        }

        void TF_ART::handleOneIteration()
        {
            projectionNumber = 0;
            for(directionIndex = 0; directionIndex < projectionSet.GetDirectionCount(); directionIndex++)
            {
                projectionProperties.resize(projectionSet.GetFocusCount(directionIndex));
                for(projectionPerDirection = 0; projectionPerDirection < projectionSet.GetFocusCount(directionIndex); projectionPerDirection++)
                {
                    if(parameterSet->shouldSkipFocalPosition(projectionPerDirection))
                        continue;

                    projectionIndex = projectionSet.getProjectionIndex(directionIndex, projectionPerDirection);

                    measuredProjection->setObjectOnCPU(projectionDataSource->getProjectionImage(projectionIndex));

                    auto props = projectionDataSource->GetProjectionProperties(projectionIndex);

                    if(parameterSet->shouldSkipDirection(props.GetTiltAngle()))
                        break;

                    if(parameterSet->shouldUseAutofocus())
                        autofocus->correctFocus(props, projectionPerDirection);

                    projectionProperties[projectionPerDirection] = props;

                    geometricSetup->setScannerGeometry(projectionDataSource->getScannerGeometry(projectionIndex, framework->getParameterSet()));

                    projectionNumber++;
                    logProjectionInformationIfRequested(projectionNumber, props);
                    writeOutProjectionIfRequested(projectionDataSource->getProjectionImage(projectionIndex));
                    handleForwardProjection();
                    compansateLongObject();
                    computeProjectedError();
                }

                for(projectionPerDirection = 0; projectionPerDirection < projectionSet.GetFocusCount(directionIndex); projectionPerDirection++)
                {
                    geometricSetup->setScannerGeometry(projectionDataSource->getScannerGeometry(projectionIndex, framework->getParameterSet()));
                    handleBackProjection();
                }

                writeOutIntermediateVolumeIfRequested("direction_");

            }
            writeIterationResulIfRequested();
        }

        void TF_ART::initializeProjectionSet()
        {
            projectionSet.SetProjectionIndices(projectionDataSource->CollectAllValidIndices(), framework->getParameterSet()->SkipProjectionList());
        }

        void TF_ART::initializeComputeKernel()
        {
            forwardProjectionOperator = new STEMForwardProjectionOperator(framework, projectionResolution, geometricSetup, baseScannerGeometry.getSourceBase(), volume, framework->getParameterSet()->get<ForwardProjectionParameterSet>()->Samples());

            measuredProjection = new GPUMapped<Image>(clContext, projectionResolution);

            if(framework->getParameterSet()->get<AlgorithmParameterSet>()->UseLongObjectCompensation())
            {
                longObjectCompensationKernel = new VirtualProjectionKernel(framework, forwardProjectionOperator->getRayLength());
                longObjectCompensationKernel->setProjection(forwardProjectionOperator->getOutput());
                compareKernel = new CompareKernel(framework, projectionResolution, measuredProjection, longObjectCompensationKernel->getProjection());
            }
            else
                compareKernel = new CompareKernel(framework, projectionResolution, measuredProjection, forwardProjectionOperator->getOutput());

            rmsKernel = new RMSKernel(framework, compareKernel->getOutput());
            initializeBackProjectionOperator();
        }

        void TF_ART::initializeBackProjectionOperator()
        {
            switch(parameterSet->getBackProjectionMode())
            {
            case TF_ART_ParameterSet::REGULARIZATION:
                backprojectionOperator = new Regularization_BackProjectionOperator(framework, geometricSetup, volume, compareKernel->getOutput(), framework->getParameterSet()->get<AlgorithmParameterSet>()->Lambda());
                break;

            case TF_ART_ParameterSet::ADJOINT:
                backprojectionOperator = new Adjoint_BackProjectionOperator(framework, geometricSetup, volume, compareKernel->getOutput(), framework->getParameterSet()->get<AlgorithmParameterSet>()->Lambda());
                break;

            default:
                throw Exception("illegal back projection mode for this reconstruction algorithm");
                break;
            }
        }

        void TF_ART::deleteComputeKernel()
        {
            if(framework->getParameterSet()->get<AlgorithmParameterSet>()->UseLongObjectCompensation())
                delete longObjectCompensationKernel;
            delete compareKernel;
            delete rmsKernel;
        }

        void TF_ART::allocateDataBuffer()
        {
            ownedVolume = new Volume(VolumeProperties(VoxelType::FLOAT_32, *framework->getParameterSet()->get<VolumeParameterSet>(), framework->getParameterSet()->get<HardwareParameterSet>()->SubVolumeCount()), framework->getParameterSet()->get<VolumeParameterSet>()->InitValue());
            volume = new GPUMappedVolume(clContext, ownedVolume);


            projectionDataSource->writeToLog();

            int residualCount = projectionSet.FindMaximumFocusCountPerDirection();
            residuals.resize(residualCount);
        }

        void TF_ART::freeDataBuffer()
        {
            for(auto it = residuals.begin(); it != residuals.end(); ++it)
            {
                delete *it;
            }
            residuals.clear();
            projectionProperties.clear();
            delete volume;
            delete ownedVolume;
        }

        void TF_ART::initializeMicroscopeGeometry()
        {
            baseScannerGeometry = ScannerGeometry(projectionDataSource->getProjectionType(), projectionDataSource->getResolution());
            geometricSetup = new GeometricSetup(baseScannerGeometry, ownedVolume);
        }

        void TF_ART::handleForwardProjection()
        {
            forwardProjectionOperator->SetProjectionProperties(projectionProperties[projectionPerDirection]);
            forwardProjectionOperator->run();
            writeOutVirtualProjectionIfRequested();
        }

        void TF_ART::compansateLongObject()
        {
            if(!framework->getParameterSet()->get<AlgorithmParameterSet>()->UseLongObjectCompensation())
                return;

            float traversalLength = geometricSetup->getScannerGeometry().getTraversalLengthThroughBoundingBox(volume->Properties().GetVolumeBoundingBox());
            longObjectCompensationKernel->setConstantLength(traversalLength);
            longObjectCompensationKernel->run();
        }

        void TF_ART::computeProjectedError()
        {
            compareKernel->run();

            if(framework->getParameterSet()->get<DebugParameterSet>()->DisplayProjectionRMS())
            {
                rmsKernel->run();
                LOGGER_IMP("residual rms: " << rmsKernel->getRootMeanSquare());
            }

            writeOutResidualsIfRequested();
            compareKernel->getOutput()->ensureIsUpToDateOnCPU();
            auto oldResidual = residuals[projectionPerDirection];
            residuals[projectionPerDirection] = new Image(compareKernel->getOutput()->getObjectOnCPU());
            delete oldResidual;
        }

        void TF_ART::handleBackProjection()
        {
            compareKernel->getOutput()->setObjectOnCPU(residuals[projectionPerDirection]);
            backprojectionOperator->SetProjectionProperties(projectionProperties[projectionPerDirection]);
            backprojectionOperator->run();
        }

        void TF_ART::writeRunInformationToLog()
        {
            LOGGER("Starting reconstruction");
            LOGGER_INFO_FORMATTED("Number of iterations" << framework->getParameterSet()->get<AlgorithmParameterSet>()->NumberOfIterations());
            LOGGER_INFO_FORMATTED("Number of projections" << projectionSet.getNumberOfProjections());
        }

        void TF_ART::logProjectionInformationIfRequested(unsigned int projectionNumber, const TFProjectionProperties& projectionProperties)
        {
            if(framework->getParameterSet()->get<DebugParameterSet>()->PrintProgressInfo())
            {
                LOGGER_IMP("Projection number: " << projectionNumber + 1
                           << "/"
                           << projectionSet.getNumberOfProjections()
                           << " (" << projectionProperties << ") "
                           << "iteration: "
                           << currentIteration
                           << "/"
                           << framework->getParameterSet()->get<AlgorithmParameterSet>()->NumberOfIterations());
            }
        }

        void TF_ART::writeOutProjectionIfRequested(Image* projection)
        {
            if(framework->getParameterSet()->get<DebugParameterSet>()->WriteProjections())
            {
                std::string filename = (boost::format("debug/projection_%1%_%2%") % currentIteration % projectionNumber).str();
                ImageSerializer::writeImage(filename.c_str(), projection, ImageSerializer::ImageFormat::TIFF_GRAY_32);
            }
        }

        void TF_ART::writeOutVirtualProjectionIfRequested()
        {
            if(!framework->getParameterSet()->get<DebugParameterSet>()->WriteVirtualProjections())
                return;

            std::string filename = (boost::format("debug/virtualProjection_%1%_%2%") % currentIteration % projectionNumber).str();
            ImageSerializer::writeImage(filename, forwardProjectionOperator->getOutput(), ImageSerializer::TIFF_GRAY_32);

            filename = (boost::format("debug/traversalLength_%1%") % projectionNumber).str();
            ImageSerializer::writeImage(filename, forwardProjectionOperator->getRayLength(), ImageSerializer::TIFF_GRAY_32);
        }

        void TF_ART::writeOutResidualsIfRequested()
        {
            if(!framework->getParameterSet()->get<DebugParameterSet>()->WriteResiduals())
                return;

            std::string filename = (boost::format("debug/residuals_%1%_%2%") % currentIteration % projectionNumber).str();
            ImageSerializer::writeImage(filename, compareKernel->getOutput(), ImageSerializer::TIFF_GRAY_32);
        }

        void TF_ART::writeOutIntermediateVolumeIfRequested(std::string filenamebase)
        {
            if(!framework->getParameterSet()->get<DebugParameterSet>()->WriteIntermediateVolumes())
                return;

            std::string filename = (boost::format(filenamebase + "%1%_%2%_%3%.mrc") % currentIteration % directionIndex % projectionPerDirection).str();
            MRCWriter writer;
            volume->ensureIsUpToDateOnCPU();
            const OutputFormatParameterSet format;
            writer.write(volume->getObjectOnCPU(), filename, &format);
        }

        void TF_ART::writeIterationResulIfRequested()
        {
            if(!framework->getParameterSet()->get<DebugParameterSet>()->WriteIntermediateVolumesAfterIterations())
                return;

            std::string filename = (boost::format("iteration_%1%.mrc") % currentIteration).str();
            MRCWriter writer;
            volume->ensureIsUpToDateOnCPU();
            const OutputFormatParameterSet format;
            writer.write(volume->getObjectOnCPU(), filename, &format);
        }

        void TF_ART::writeFinalVolume()
        {
            const OutputFormatParameterSet format;
            framework->getVolumeSerializer()->write(volume, framework->getParameterSet()->get<IOParameterSet>()->OutputVolumeFileName().string(), "", &format);
        }

    }
}