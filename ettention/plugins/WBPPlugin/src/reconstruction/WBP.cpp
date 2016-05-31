#include "stdafx.h"
#include "WBP.h"
#include <algorithm/imagemanipulation/convolution/ConvolutionOperator.h>
#include <algorithm/longobjectcompensation/ComputeRayLengthKernel.h>
#include <algorithm/projections/back/BackProjectionOperator.h>
#include <algorithm/imagemanipulation/GenerateRamLakFilterKernel.h>
#include <framework/Framework.h>
#include <io/datasource/ImageStackDatasource.h>
#include <io/serializer/ImageSerializer.h>
#include <model/geometry/GeometricSetup.h>
#include "model/geometry/ParallelScannerGeometry.h"
#include "model/RamLakGenerator.h"
#include "model/volume/FloatVolume.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include <setup/parameterset/HardwareParameterSet.h>
#include "setup/WBPParameterSet.h"

namespace ettention
{
    namespace wbp
    {
        WBP::WBP(Framework* framework)
            : ReconstructionAlgorithm(framework)
        {
            // open the input stack specified by a configuration file and extract projection geometry
            input = framework->openInputStack();
            
            // allocate an output volume, resolution is specified by configuration
            auto volumeOnCpu = new FloatVolume((VolumeParameterSet*)framework->getParameterSet()->get<VolumeParameterSet>(), framework->getParameterSet()->get<HardwareParameterSet>()->getSubVolumeCount());

            // allocate the scanner geometry
            geometricSetup = new GeometricSetup(new ParallelScannerGeometry(input), volumeOnCpu);

            // allocate objects on the GPU
            volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
            projection = new GPUMapped<Image>(framework->getOpenCLStack(), input->getResolution());

            // allocate compute kernels and generate filter mask
            filterSize = 128;
            ramLakKernel = new GenerateRamLakKernel(framework, filterSize);
            ramLakKernel->run();
            convolution = new ConvolutionOperator(framework, ramLakKernel->getOutput(), projection, true);

            // compute depth in view direction of the volume, per pixel (for normalization)
            auto rayLengthImage = new GPUMapped<Image>(framework->getOpenCLStack(), input->getResolution());
            rayLengthKernel = new ComputeRayLengthKernel(framework, geometricSetup, rayLengthImage);

            // allocate back projection operator
            nProjections = input->getNumberOfProjections();
            backprojection = new BackProjectionOperator(framework, geometricSetup, volume, nullptr);
            backprojection->setInput(convolution->getOutput(), rayLengthImage, nProjections);

            // organize GPU memory for best performance
            optimizeMemoryUsage();
        }

        WBP::~WBP()
        {
            delete convolution;
            delete ramLakKernel;
            delete rayLengthKernel->getOutput();
            delete rayLengthKernel;
            delete backprojection;
            delete geometricSetup;
            delete projection;
            delete volume->getObjectOnCPU();
            delete volume;
            delete input;
        }

        void WBP::run()
        {
            runNiterations(input->getNumberOfProjections());
        }

        void WBP::runNiterations(unsigned int iterationsCount)
        {
            for( unsigned int i = 0; i < iterationsCount; ++i )
            {
                projection->setObjectOnCPU(input->getProjectionImage(i));
                ScannerGeometry* geometry = input->createScannerGeometry(i, framework->getParameterSet());
                geometricSetup->setScannerGeometry(geometry);
                convolution->execute();
                rayLengthKernel->run();
                backprojection->execute();
            }
        }

        void WBP::runSingleIteration()
        {
            runNiterations();
        }

        GPUMappedVolume* WBP::getReconstructedVolume() const
        {
            return volume;
        }

        void WBP::exportGeometryTo(Visualizer* visualizer)
        {
            throw Exception("Geometry export not supported by WBP!");
        }

        void WBP::optimizeMemoryUsage()
        {
            if(framework->getParameterSet()->get<HardwareParameterSet>()->getSubVolumeCount())
                return;

            volume->changeSubVolumeSizeToPartOfTotalMemory(1.0f);
        }
    }
}