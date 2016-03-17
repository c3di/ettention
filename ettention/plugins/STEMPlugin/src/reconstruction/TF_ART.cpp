#include "stdafx.h"
#include "TF_ART.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "io/serializer/VolumeSerializer.h"
#include "reconstruction/operators/Adjoint_BackProjection_Operator.h"
#include "reconstruction/operators/Regularization_BackProjection_Operator.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/DebugParameterSet.h"
#include "setup/parameterset/ForwardProjectionParameterSet.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/parameterset/InputParameterSet.h"
#include "model/STEMScannerGeometry.h"

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

            auto numberOfIterations = framework->getParameterSet()->get<AlgorithmParameterSet>()->getNumberOfIterations();
            for(currentIteration = 1; currentIteration <= numberOfIterations; currentIteration++)
            {
                handleOneIteration();
            }
            // writeFinalVolume();
        }

        GPUMappedVolume* TF_ART::getReconstructedVolume() const
        {
            return volume;
        }

        void TF_ART::exportGeometryTo(Visualizer* visualizer)
        {
            throw Exception("Geometry export not supported by TF_ART!");
        }

        void TF_ART::handleOneIteration()
        {
            projectionNumber = 0;
            for (directionIndex = 0; directionIndex < projectionSet.getDirectionCount(); directionIndex++)
            {
                projectionProperties.resize(projectionSet.getFocusCount(directionIndex));
                for (projectionPerDirection = 0; projectionPerDirection < projectionSet.getFocusCount(directionIndex); projectionPerDirection++)
                {
                    if(parameterSet->shouldSkipFocalPosition(projectionPerDirection))
                        continue;

                    projectionIndex = projectionSet.getProjectionIndex(directionIndex, projectionPerDirection);

                    measuredProjection->setObjectOnCPU(projectionDataSource->getProjectionImage(projectionIndex));

                    auto props = (STEMScannerGeometry*) projectionDataSource->GetScannerGeometry(projectionIndex);

                    if(parameterSet->shouldSkipDirection(props->getTiltAngle()))
                        break;

                    if(parameterSet->shouldUseAutofocus())
                        autofocus->correctFocus(props, projectionPerDirection);

                    projectionProperties[projectionPerDirection] = props;

                    geometricSetup->setScannerGeometry(props->clone());
                    forwardProjectionOperator->setScannerGeometry(props);

                    projectionNumber++;
                    logProjectionInformationIfRequested(props);
                    writeOutProjectionIfRequested(projectionDataSource->getProjectionImage(projectionIndex));
                    handleForwardProjection();
                    compansateLongObject();
                    computeProjectedError();
                }

                for (projectionPerDirection = 0; projectionPerDirection < projectionSet.getFocusCount(directionIndex); projectionPerDirection++)
                {
                    auto props = projectionProperties[projectionPerDirection];
                    geometricSetup->setScannerGeometry(props->clone());
                    handleBackProjection();
                }

                writeOutIntermediateVolumeIfRequested("direction_");

            }
            writeIterationResulIfRequested();
        }

        void TF_ART::initializeProjectionSet()
        {
            projectionSet.setProjectionIndices(projectionDataSource->CollectAllValidIndices(), framework->getParameterSet()->get<InputParameterSet>()->getSkipProjectionList());
        }

        void TF_ART::initializeComputeKernel()
        {
            forwardProjectionOperator = new STEMForwardProjectionOperator(framework, projectionResolution, geometricSetup, baseScannerGeometry->getSourceBase(), volume, framework->getParameterSet()->get<ForwardProjectionParameterSet>()->getSamplesNumber());
            forwardProjectionOperator->setOutput(virtualProjection, rayLengthImage);
            measuredProjection = new GPUMapped<Image>(clContext, projectionResolution);

            if(framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation())
            {
                longObjectCompensationKernel = new VirtualProjectionKernel(framework, forwardProjectionOperator->getVirtualProjection(), forwardProjectionOperator->getRayLength());
                compareKernel = new CompareKernel(framework, projectionResolution, measuredProjection, longObjectCompensationKernel->getProjection());
            }
            else
                compareKernel = new CompareKernel(framework, projectionResolution, measuredProjection, forwardProjectionOperator->getVirtualProjection());

            rmsKernel = new RMSKernel(framework, compareKernel->getOutput());
            initializeBackProjectionOperator();
        }

        void TF_ART::initializeBackProjectionOperator()
        {
            switch(parameterSet->getBackProjectionMode())
            {
            case TF_ART_ParameterSet::REGULARIZATION:
                backprojectionOperator = new Regularization_BackProjectionOperator(framework, geometricSetup, volume, compareKernel->getOutput(), framework->getParameterSet()->get<AlgorithmParameterSet>()->getLambda());
                break;

            case TF_ART_ParameterSet::ADJOINT:
                backprojectionOperator = new Adjoint_BackProjectionOperator(framework, geometricSetup, volume, compareKernel->getOutput(), framework->getParameterSet()->get<AlgorithmParameterSet>()->getLambda());
                break;

            default:
                throw Exception("illegal back projection mode for this reconstruction algorithm");
                break;
            }
        }

        void TF_ART::deleteComputeKernel()
        {
            if(framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation())
                delete longObjectCompensationKernel;
            delete compareKernel;
            delete rmsKernel;
        }

        void TF_ART::allocateDataBuffer()
        {
            ownedVolume = new FloatVolume(framework->getParameterSet()->get<VolumeParameterSet>()->getResolution(), framework->getParameterSet()->get<VolumeParameterSet>()->getInitValue(), framework->getParameterSet()->get<HardwareParameterSet>()->getSubVolumeCount());
            volume = new GPUMappedVolume(clContext, ownedVolume);
            
            virtualProjection = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
            rayLengthImage = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);

            projectionDataSource->writeToLog();

            int residualCount = projectionSet.findMaximumFocusCountPerDirection();
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

            delete virtualProjection;
            delete rayLengthImage;
        }

        void TF_ART::initializeMicroscopeGeometry()
        {
            baseScannerGeometry = (STEMScannerGeometry*)projectionDataSource->GetBaseScannerGeometry()->clone();
            geometricSetup = new GeometricSetup( baseScannerGeometry, ownedVolume );
        }

        void TF_ART::handleForwardProjection()
        {
            // geometricSetup->setScannerGeometry( projectionProperties[projectionPerDirection] );
            // forwardProjectionOperator->SetProjectionProperties();
            forwardProjectionOperator->run();
            writeOutVirtualProjectionIfRequested();
        }

        void TF_ART::compansateLongObject()
        {
            if(!framework->getParameterSet()->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation())
                return;

            auto volumeBoundingBox = volume->getObjectOnCPU()->getProperties().getVolumeBoundingBox();
            auto traversalLength = geometricSetup->getScannerGeometry()->getTraversalLengthThroughBoundingBox(volumeBoundingBox);

            longObjectCompensationKernel->setConstantLength(traversalLength);
            longObjectCompensationKernel->run();
        }

        void TF_ART::computeProjectedError()
        {
            compareKernel->run();

            if(framework->getParameterSet()->get<DebugParameterSet>()->shouldDisplayProjectionRMS())
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
            backprojectionOperator->setScannerGeometry(projectionProperties[projectionPerDirection]);
            backprojectionOperator->run();
        }

        void TF_ART::writeRunInformationToLog()
        {
            LOGGER("Starting reconstruction");
            LOGGER_INFO_FORMATTED("Number of iterations" << framework->getParameterSet()->get<AlgorithmParameterSet>()->getNumberOfIterations());
            LOGGER_INFO_FORMATTED("Number of projections" << projectionSet.getNumberOfProjections());
        }

        void TF_ART::logProjectionInformationIfRequested( STEMScannerGeometry* scannerGeometry)
        {
            if(framework->getParameterSet()->get<DebugParameterSet>()->shouldPrintProgressInfo())
            {
                LOGGER_IMP("Projection number: " << projectionNumber + 1
                           << "/"
                           << projectionSet.getNumberOfProjections()
                           << " (" << projectionProperties[projectionPerDirection] << ") "
                           << "iteration: "
                           << currentIteration
                           << "/"
                           << framework->getParameterSet()->get<AlgorithmParameterSet>()->getNumberOfIterations());
            }
        }

        void TF_ART::writeOutProjectionIfRequested(Image* projection)
        {
            if(framework->getParameterSet()->get<DebugParameterSet>()->shouldWriteProjections())
            {
                std::string filename = (boost::format("debug/projection_%1%_%2%") % currentIteration % projectionNumber).str();
                ImageSerializer::writeImage(filename.c_str(), projection, ImageSerializer::ImageFormat::TIFF_GRAY_32);
            }
        }

        void TF_ART::writeOutVirtualProjectionIfRequested()
        {
            if(!framework->getParameterSet()->get<DebugParameterSet>()->shouldWriteVirtualProjections())
                return;

            std::string filename = (boost::format("debug/virtualProjection_%1%_%2%") % currentIteration % projectionNumber).str();
            ImageSerializer::writeImage(filename, forwardProjectionOperator->getVirtualProjection(), ImageSerializer::TIFF_GRAY_32);

            filename = (boost::format("debug/traversalLength_%1%") % projectionNumber).str();
            ImageSerializer::writeImage(filename, forwardProjectionOperator->getRayLength(), ImageSerializer::TIFF_GRAY_32);
        }

        void TF_ART::writeOutResidualsIfRequested()
        {
            if(!framework->getParameterSet()->get<DebugParameterSet>()->shouldWriteResiduals())
                return;

            std::string filename = (boost::format("debug/residuals_%1%_%2%") % currentIteration % projectionNumber).str();
            ImageSerializer::writeImage(filename, compareKernel->getOutput(), ImageSerializer::TIFF_GRAY_32);
        }

        void TF_ART::writeOutIntermediateVolumeIfRequested(std::string filenamebase)
        {
            if(!framework->getParameterSet()->get<DebugParameterSet>()->shouldWriteIntermediateVolumes())
                return;

            std::string filename = (boost::format(filenamebase + "%1%_%2%_%3%.mrc") % currentIteration % directionIndex % projectionPerDirection).str();
            MRCWriter writer;
            volume->ensureIsUpToDateOnCPU();
            const OutputParameterSet format;
            writer.write(volume->getObjectOnCPU(), filename, &format);
        }

        void TF_ART::writeIterationResulIfRequested()
        {
            if(!framework->getParameterSet()->get<DebugParameterSet>()->shouldWriteIntermediateVolumesAfterIterations())
                return;

            std::string filename = (boost::format("iteration_%1%.mrc") % currentIteration).str();
            MRCWriter writer;
            volume->ensureIsUpToDateOnCPU();
            const OutputParameterSet format;
            writer.write(volume->getObjectOnCPU(), filename, &format);
        }

        void TF_ART::writeFinalVolume()
        {
            const OutputParameterSet format;
            framework->getVolumeSerializer()->write(volume, framework->getParameterSet()->get<OutputParameterSet>()->getFilename().string(), "", &format);
        }

    }
}