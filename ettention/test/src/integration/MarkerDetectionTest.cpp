#include "stdafx.h"

#include "framework/test/ImageManipulationTestBase.h"

#include "algorithm/imagestatistics/particlepattern/PatternByAveraging.h"

#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetector.h"
#include "algorithm/imagestatistics/particledetection/CrossCorrelationMarkerDetectorAveragingPattern.h"
#include "algorithm/imagestatistics/particledetection/ConvolutionBasedDetectorWithSecondOrderGaussian.h"

#include "algorithm/imagestatistics/particledetection/ParticleExtractor.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperator.h"

#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"
#include "gpu/GPUMapped.h"
#include "model/image/ImageComparator.h"

#include <iostream>
#include <fstream>

using namespace ettention;

class ParticleDetectionTest : public ImageManipulationTestBase
{
public:

    const std::string inputDS1 = std::string("120604_C4dep_10_2048_z11");
	const std::string inputDS2 = std::string("MYC_FA_D5_4096_z1");
	const std::string inputDS3 = std::string("HPFCere2a_z1");
	const std::string inputDS4 = std::string("SKBR3_EGF-AU_ERB2-B_S-QD_033_1024x884");

	const std::string markerDS1 = std::string("marker_from_C4_black_23");
	const std::string markerDS2 = std::string("marker_from_MYC_black_63");
	const std::string markerDS3 = std::string("marker_from_HPF_white_12");
	const std::string markerDS4square = std::string("particle_from_SKBR3_black_11");
	const std::string markerDS4rectan = std::string("particle_from_SKBR3_black_9x7");

	const std::string groundDS1 = std::string("ground_particles_C4dep.txt");
	const std::string groundDS2 = std::string("ground_particles_MYC.txt");
	const std::string groundDS3 = std::string("ground_particles_source.txt");
	const std::string groundDS4 = std::string("ground_particles_SKBR3.txt");

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

        auto particles = ParticleExtractor::loadFromFile(this->makeInputFilePath(groundTruth, ""));
        for(int i = 0; i < count; ++i)
        {
            guess.push_back(Vec2ui(particles.data->at(i)));
        }
        delete particles.data;

        auto *srcImage = this->loadInputImageFileToGPU(filename);
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
        std::string outputBestParticleFilename = "_" + name + "_5%";
        std::string outputAllRefinedParticleFilename = "_" + name + "_all_ref";
        std::string outputBestRefinedParticleFilename = "_" + name + "_5%_ref";

        auto *srcImage = this->loadInputImageFileToGPU(inputFilenameSource);
        auto *patternImage = generateAvgPattern(inputFilenameSource, groundTruth, count);

        auto *detector = new CrossCorrelationMarkerDetector(framework, srcImage->getResolution(), patternImage);

        detector->process(srcImage);
        debugImageOut(name, srcImage, patternImage, detector->getParticlesMap());
        saveImageAsFile(name + "_3ref", detector->getRefinedByCircularScoreParticlesMap());

        detector->dumpPercentOfParticlesInFile(this->makeOutputFilePath(outputAllParticleFilename, "txt"));
        detector->dumpPercentOfParticlesInFile(this->makeOutputFilePath(outputBestParticleFilename, "txt"), 5);

        ParticleExtractor::extractAndSaveToFileAll(detector->getRefinedByCircularScoreParticlesMap(), this->makeOutputFilePath(outputAllRefinedParticleFilename, "txt"), detector->getParticleSize(), detector->detectingMarkerCenter());
        ParticleExtractor::extractAndSaveToFilePercent(detector->getRefinedByCircularScoreParticlesMap(), this->makeOutputFilePath(outputBestRefinedParticleFilename, "txt"), detector->getParticleSize(), detector->detectingMarkerCenter(), 5);

        delete detector;

        delete patternImage;
        delete srcImage;
    }

    virtual void correlationTestCase(std::string name, std::string reference, std::string candidate)
    {
        std::string inputFilenameSource = reference;
        std::string inputFilenamePattern = candidate;
        std::string outputAllParticleFilename = "_" + name + "_all";
        std::string outputBestParticleFilename = "_" + name + "_5%";
        std::string outputAllRefinedParticleFilename = "_" + name + "_all_ref";
        std::string outputBestRefinedParticleFilename = "_" + name + "_5%_ref";

        auto *srcImage = this->loadInputImageFileToGPU(inputFilenameSource);
        auto *patternImage = this->loadInputImageFileToGPU(inputFilenamePattern);

        auto *detector = new CrossCorrelationMarkerDetector(framework, srcImage->getResolution(), patternImage);

        detector->process(srcImage);
        debugImageOut(name, srcImage, patternImage, detector->getParticlesMap());
        saveImageAsFile(name + "_3ref", detector->getRefinedByCircularScoreParticlesMap());

        detector->dumpPercentOfParticlesInFile(this->makeOutputFilePath(outputAllParticleFilename, "txt"));
        detector->dumpPercentOfParticlesInFile(this->makeOutputFilePath(outputBestParticleFilename, "txt"), 5);

        ParticleExtractor::extractAndSaveToFileAll(detector->getRefinedByCircularScoreParticlesMap(), this->makeOutputFilePath(outputAllRefinedParticleFilename, "txt"), detector->getParticleSize(), detector->detectingMarkerCenter());
        ParticleExtractor::extractAndSaveToFilePercent(detector->getRefinedByCircularScoreParticlesMap(), this->makeOutputFilePath(outputBestRefinedParticleFilename, "txt"), detector->getParticleSize(), detector->detectingMarkerCenter(), 5);

        delete detector;

        delete patternImage;
        delete srcImage;
    }

    virtual void convolutionTestCase(std::string name, std::string input, unsigned int particleDiameter)
    {
        std::string inputFilename = input;
        std::string outputAllParticleFilename = "_" + name + "_all";
        std::string outputBestParticleFilename = "_" + name + "_5%";
        std::string outputImageFilename = name + "_result";
        unsigned int diameter = particleDiameter;

        auto *srcImage = this->loadInputImageFileToGPU(inputFilename);

        auto *detector = new ConvolutionBasedDetectorWithSecondOrderGaussian(framework, srcImage, diameter);

        detector->process();
        detector->doDebugOutput(this->pathToOutputData + "xDebug_");
        saveImageAsFile(outputImageFilename, detector->getParticlesMap());

        detector->dumpPercentOfParticlesInFile(this->makeOutputFilePath(outputAllParticleFilename, "txt"));
        detector->dumpPercentOfParticlesInFile(this->makeOutputFilePath(outputBestParticleFilename, "txt"), 5);

        //detector->doDebugOutput(this->pathToOutputData + "_" + name);

        delete detector;

        delete srcImage;
    }
};

//
// Marker-Particle Detection Tests
//

TEST_F(ParticleDetectionTest, PD_NCC_Averaging)
{
#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS1:");
#endif
    this->averagingTestCase("avg_DS1", inputDS1, groundDS1, 3);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS2:");
#endif
    this->averagingTestCase("avg_DS2", inputDS2, groundDS2, 3);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS3:");
#endif
    this->averagingTestCase("avg_DS3", inputDS3, groundDS3, 3);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS4:");
#endif
    this->averagingTestCase("avg_DS4", inputDS4, groundDS4, 3);

#ifdef _WINDOWS
    SetConsoleTitleA("Finished with Averaging detector test!");
#endif
}

TEST_F(ParticleDetectionTest, PD_NCC_General)
{
#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS1:");
#endif
    this->correlationTestCase("ncc_DS1", inputDS1, markerDS1);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS2:");
#endif
    this->correlationTestCase("ncc_DS2", inputDS2, markerDS2);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS3:");
#endif
    this->correlationTestCase("ncc_DS3", inputDS3, markerDS3);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS4:");
#endif
    this->correlationTestCase("ncc_DS4_1_sqre", inputDS4, markerDS4square);
    this->correlationTestCase("ncc_DS4_2_rect", inputDS4, markerDS4rectan);

#ifdef _WINDOWS
    SetConsoleTitleA("Finished with NCC detector test!");
#endif
}

TEST_F(ParticleDetectionTest, PD_Convolution_Cao)
{
#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS1:");
#endif
    this->convolutionTestCase("cao_DS1", inputDS1, 23);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS2:");
#endif
    this->convolutionTestCase("cao_DS2", inputDS2, 63);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS3:");
#endif
    this->convolutionTestCase("cao_DS3", inputDS3 + "_inv", 11);

#ifdef _WINDOWS
    SetConsoleTitleA("Processing DS4:");
#endif
    this->convolutionTestCase("cao_DS4", inputDS4 + "_inv", 11);

#ifdef _WINDOWS
    SetConsoleTitleA("Finished with Cao detector test!");
#endif
}
