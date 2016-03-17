#pragma once
#include "algorithm/imagemanipulation/CompareKernel.h"
#include "algorithm/imagemanipulation/RMSKernel.h"
#include "algorithm/longobjectcompensation/VirtualProjectionKernel.h"
#include "algorithm/reconstruction/ReconstructionAlgorithm.h"
#include "alignment/autofocus/AutomaticFocusComputation.h"
#include "reconstruction/operators/STEMForwardProjectionOperator.h"
#include "reconstruction/operators/TFS_BackProjection_Operator.h"

namespace ettention
{
	class ScannerGeometry;

	namespace stem
    {
		class STEMScannerGeometry;

        class PLUGIN_API TF_ART : public ReconstructionAlgorithm
        {
        public:
            TF_ART(Framework* framework);
            TF_ART(TF_Datasource* projectionDataSource, Framework* framework);

            virtual ~TF_ART();

            void run() override;
            GPUMappedVolume* getReconstructedVolume() const override;
            void exportGeometryTo(Visualizer* visualizer) override;

        protected:
            virtual void handleOneIteration();

            virtual void handleForwardProjection();
            virtual void compansateLongObject();
            virtual void computeProjectedError();

            void handleBackProjection();

            void allocateDataBuffer();
            void freeDataBuffer();

            void initializeMicroscopeGeometry();

            void initializeProjectionSet();
            void initializeComputeKernel();
            void initializeBackProjectionOperator();
            void deleteComputeKernel();

        protected:
			void logProjectionInformationIfRequested( STEMScannerGeometry* geometricSetup );

            void writeOutIntermediateVolumeIfRequested(std::string filenamebase);
            void writeIterationResulIfRequested();
            void writeRunInformationToLog();
            void writeOutProjectionIfRequested(Image* projection);
            void writeOutVirtualProjectionIfRequested();
            void writeOutResidualsIfRequested();
            void writeFinalVolume();

            // Parameter Set
            const TF_ART_ParameterSet* parameterSet;

            // Algorithm Data Structures
            Volume* ownedVolume;
            GPUMappedVolume* volume;

            bool ownsProjectionDataSource;
            TF_Datasource* projectionDataSource;
            GeometricSetup* geometricSetup;
            STEMScannerGeometry* baseScannerGeometry;
            ProjectionSetRandomDirectionSequentialFocus projectionSet;

            // Alignment
            AutomaticFocusComputation* autofocus;

            // Computer Kernel & GPU Abstraction
            CLAbstractionLayer* clContext;

            CompareKernel* compareKernel;
            RMSKernel* rmsKernel;
            STEMForwardProjectionOperator* forwardProjectionOperator;
            VirtualProjectionKernel* longObjectCompensationKernel;
            TFS_BackProjectionOperator* backprojectionOperator;

            // Data Buffers
			GPUMapped<Image>* measuredProjection;
			GPUMapped<Image>* virtualProjection;
			GPUMapped<Image>* rayLengthImage;
			std::vector<Image*> residuals;
			std::vector<STEMScannerGeometry*> projectionProperties;

            // Working Variables
            unsigned int currentIteration;
            unsigned int volumeSize;

            Vec2ui projectionResolution;
            unsigned int projectionNumber;
            HyperStackIndex projectionIndex;

            unsigned int directionIndex;
            unsigned int projectionPerDirection;
        };

    }
}