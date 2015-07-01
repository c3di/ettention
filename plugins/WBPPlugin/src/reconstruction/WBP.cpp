#include "stdafx.h"
#include "WBP.h"
#include <algorithm/imagemanipulation/convolution/ConvolutionOperator.h>
#include <algorithm/longobjectcompensation/ComputeRayLengthKernel.h>
#include <algorithm/projections/back/BackProjectionOperator.h>
#include <algorithm/imagemanipulation/GenerateRamLakFilterKernel.h>
#include <io/datasource/ImageStackDatasource.h>
#include <io/serializer/ImageSerializer.h>
#include <model/geometry/GeometricSetup.h>
#include <setup/ParameterSet/HardwareParameterSet.h>
#include "setup/WBPParameterSet.h"

#include "model/RamLakGenerator.h"

namespace ettention
{
    namespace wbp
    {
        WBP::WBP(Framework* framework) : ReconstructionAlgorithm(framework)
        {
            // open the input stack specified by a configuration file and extract projection geometry
            input = framework->openInputStack();
            
            // allocate an output volume, resolution is specified by configuration
            auto volumeOnCpu = new  Volume(VoxelType::FLOAT_32, framework->getParameterSet());

            // allocate the scanner geometry
            geometricSetup = new GeometricSetup(ScannerGeometry(input), volumeOnCpu);

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
            backprojection = new BackProjectionOperator(framework, geometricSetup, volume, 0);
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
            for(unsigned int i = 0; i < input->getNumberOfProjections(); ++i)
            {
                projection->setObjectOnCPU(input->getProjectionImage(i));
                ScannerGeometry geometry = input->getScannerGeometry(i, framework->getParameterSet());
                geometricSetup->setScannerGeometry(geometry);
                convolution->execute();
                rayLengthKernel->run();
                backprojection->execute();
            }
        }

        GPUMappedVolume* WBP::getReconstructedVolume() const
        {
            return volume;
        }

        void WBP::optimizeMemoryUsage()
        {
            if(framework->getParameterSet()->get<HardwareParameterSet>()->SubVolumeCount())
                return;

            volume->changeSubVolumeSizeToPartOfTotalMemory(1.0f);
        }
    }
}