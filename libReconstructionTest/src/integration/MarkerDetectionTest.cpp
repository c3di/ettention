#include "stdafx.h"
#include "framework/test/ImageManipulationTestBase.h"
#include "algorithm/imagestatistics/particlepattern/PatternByAveraging.h"
#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetector.h"
#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetectorAveragingPattern.h"
#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetectorBrightFieldPattern.h"
#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetectorDarkFieldPattern.h"
#include "algorithm/imagestatistics/particledetection/ConvolutionBasedDetectorWithSecondOrderGaussian.h"
#include "algorithm/imagestatistics/particledetection/ParticleExtractor.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperator.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "gpu/GPUMapped.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "model/image/ImageComparator.h"
#include <iostream>
#include <fstream>

namespace ettention
{
    class ParticleDetectionTest : public ImageManipulationTestBase
    {
    public:
        ParticleDetectionTest()
            : ImageManipulationTestBase(std::string("PD"), std::string(TESTDATA_DIR"/data/alignment"), std::string(TESTDATA_DIR"/work/alignment/particle_detection"))
        {
        }

        virtual void debugImageOut(std::string groupName, GPUMapped<Image> *reference, GPUMapped<Image> *candidate, GPUMapped<Image> *result)
        {
            this->saveImageAsFile(groupName + "_0src", reference);
            this->saveImageAsFile(groupName + "_1cnd", candidate);
            this->saveImageAsFile(groupName + "_2res", result);
        }

        virtual GPUMapped<Image> *generateAvgPattern(std::string filename, std::string groundTruth, int count)
        {
            std::vector<Vec2ui> guess;

            auto particles = ParticleExtractor::loadFromFile(this->makeInputFilePath(groundTruth, "txt"));
            for(int i = 0; i < count; ++i)
            {
                guess.push_back(Vec2ui(particles.data->at(i)));
            }
            delete particles.data;

            auto *srcImage = this->loadImageFileToGPU(filename);
            Image *image = PatternByAveraging::computePattern(srcImage->getObjectOnCPU(), guess, particles.size.x / 2);
            delete srcImage;

            auto *result = new GPUMapped<Image>(framework->getOpenCLStack(), image);
            result->takeOwnershipOfObjectOnCPU();

            return result;
        }

        virtual void averagingTestCase(std::string name, std::string reference, std::string groundTruth, int count)
        {
            std::string inputFilenameSource = reference;
            std::string outputAllParticleFilename = "_" + name + "_all";

            auto *srcImage = this->loadImageFileToGPU(inputFilenameSource);
            auto *patternImage = generateAvgPattern(inputFilenameSource, groundTruth, count);

            auto *detector = new CrossCorrelationMarkerDetector(framework, srcImage->getResolution(), patternImage);

            detector->process(srcImage);
            this->debugImageOut(name, srcImage, patternImage, detector->getParticlesMap());
            ParticleExtractor::saveToFileAll(detector->getParticlesList(), this->makeOutputFilePath(outputAllParticleFilename, "txt"), detector->getParticleSize(), true);

            delete detector;

            delete patternImage;
            delete srcImage;
        }

        virtual void correlationTestCase(std::string name, std::string reference, std::string candidate)
        {
            std::string inputFilenameSource = reference;
            std::string inputFilenamePattern = candidate;
            std::string outputAllParticleFilename = "_" + name + "_all";

            auto *srcImage = this->loadImageFileToGPU(inputFilenameSource);
            auto *patternImage = this->loadImageFileToGPU(inputFilenamePattern);

            auto *detector = new CrossCorrelationMarkerDetector(framework, srcImage->getResolution(), patternImage);

            detector->process(srcImage);
            this->debugImageOut(name, srcImage, patternImage, detector->getParticlesMap());
            ParticleExtractor::saveToFileAll(detector->getParticlesList(), this->makeOutputFilePath(outputAllParticleFilename, "txt"), detector->getParticleSize(), true);

            delete detector;

            delete patternImage;
            delete srcImage;
        }

        virtual void convolutionTestCase(std::string name, std::string input, unsigned int particleDiameter)
        {
            std::string inputFilename = input;
            std::string outputAllParticleFilename = "_" + name + "_all";
            std::string outputImageFilename = name + "_result";
            unsigned int diameter = particleDiameter;

            auto *srcImage = this->loadImageFileToGPU(inputFilename);

            auto *detector = new ConvolutionBasedDetectorWithSecondOrderGaussian(framework, srcImage, diameter);

            detector->process();
            this->saveImageAsFile(outputImageFilename, detector->getParticlesMap());
            ParticleExtractor::saveToFileAll(detector->getParticlesList(), this->makeOutputFilePath(outputAllParticleFilename, "txt"), detector->getParticleSize(), true);

            delete detector;

            delete srcImage;
        }
    };
}

using namespace ettention;

TEST_F(ParticleDetectionTest, NCC_Averaging)
{
    this->averagingTestCase("avg_1", "source", "ground_particles_source", 5);
    this->averagingTestCase("avg_2", "source_2_1", "ground_particles_source_2-1", 4);
    this->averagingTestCase("avg_3", "SKBR3_EGF-AU_ERB2-B_S-QD_033_1024x884", "ground_particles_SKBR3", 3);
    //this->averagingTestCase("avg_4", "MYC_FA_D5_4096_z1", "ground_particles_MYC", 3);
    this->averagingTestCase("avg_5", "120604_C4dep_10_2048_z11", "ground_particles_C4dep", 3);
}

TEST_F(ParticleDetectionTest, NCC_General)
{
    this->correlationTestCase("ncc_1", "source", "marker_from_source_12");
    this->correlationTestCase("ncc_2", "source_2_1", "marker_from_source_12");
    this->correlationTestCase("ncc_3_1_sqre", "SKBR3_EGF-AU_ERB2-B_S-QD_033_1024x884", "particle_from_SKBR3_black_11");
    this->correlationTestCase("ncc_3_2_rect", "SKBR3_EGF-AU_ERB2-B_S-QD_033_1024x884", "particle_from_SKBR3_black_9x7");
    //this->correlationTestCase("ncc_4", "MYC_FA_D5_4096_z1", "marker_pattern_black_63");
    this->correlationTestCase("ncc_5", "120604_C4dep_10_2048_z11", "marker_pattern_black_23");
}

TEST_F(ParticleDetectionTest, Convolution_SecondOrderGaussian_Cao)
{
    this->convolutionTestCase("caoD_1", "source_inv", 11);
    this->convolutionTestCase("caoD_2", "source_2_1_inv", 11);
    this->convolutionTestCase("caoD_3", "SKBR3_EGF-AU_ERB2-B_S-QD_033_1024x884_inv", 11);
    this->convolutionTestCase("caoD_5", "120604_C4dep_10_2048_z11", 23);
}
