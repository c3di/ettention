#include "stdafx.h"
#include "algorithm/longobjectcompensation/LongObjectCompensationKernel.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeReconstructionOperator.h"
#include "framework/Logger.h"
#include "framework/test/ProjectionTestBase.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "io/datasource/ImageStackDatasource.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "io/datasource/MRCStack.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "io/serializer/VolumeSerializer.h"
#include "model/volume/StackComparator.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/IOParameterSet.h"
#include "setup/parametersource/CascadingParameterSource.h"
#include "setup/parametersource/CommandLineParameterSource.h"
#include "setup/parametersource/ManualParameterSource.h"
#include "setup/parametersource/XMLParameterSource.h"

namespace ettention
{
    class SyntheticReconstructionTest : public ProjectionTestBase
    {
    public:
        SyntheticReconstructionTest()
            : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
        {
        }

        void SetUp() override
        {
            ProjectionTestBase::SetUp();

            mappedVolume = new GPUMappedVolume(clal, volume);
            residualBuffer = new CLMemBuffer(clal, projectionResolution.x * projectionResolution.y * sizeof(float));
        }

        void TearDown() override
        {
            delete mappedVolume;
            delete residualBuffer;

            ProjectionTestBase::TearDown();
        }

        void writeTiltFile(std::string directoryRoot, float minTilt, float maxTilt, float stepSize)
        {
            std::string tiltFilename = directoryRoot + "/synthetic.tilt";
            if(!boost::filesystem::exists(tiltFilename))
            {
                std::ofstream tiltFile(tiltFilename.c_str());
                if(!tiltFile.good())
                {
                    throw std::runtime_error((boost::format("Cannot write synthetic tilt file to %1%") % tiltFilename).str().c_str());
                }
                for(float tilt = minTilt; tilt < maxTilt; tilt += stepSize)
                {
                    tiltFile << tilt << std::endl;
                }
                tiltFile.close();
            }
        }

        void generateSyntheticDatasetUsingParallelForwardProjection(std::string filenameTrunk)
        {
            MRCWriter writer;
            const OutputFormatParameterSet format;
            writer.write(volume, filenameTrunk + "/ground_truth.mrc", &format);

            const float minTilt = -90.f;
            const float maxTilt = 90.f;
            const float tiltStep = 1.f;

            ParallelBeamsForwardProjectionKernel* forwardKernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, 1);
            auto projection = new GPUMapped<Image>(clal, projectionResolution);
            auto traversalLength = new GPUMapped<Image>(clal, projectionResolution);
            forwardKernel->SetOutput(projection, traversalLength);

            mappedVolume->ensureIsUpToDateOnGPU();

            writeTiltFile(filenameTrunk, minTilt, maxTilt, tiltStep);

            std::vector<float> tiltAngles;
            std::vector<std::string> filenames;
            for(float tiltPosition = minTilt; tiltPosition < maxTilt; tiltPosition += tiltStep)
            {
                geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(tiltPosition));
                projection->getBufferOnGPU()->clear();
                forwardKernel->run();

                std::stringstream fname;
                fname << filenameTrunk << "/projection" << tiltPosition;
                ImageSerializer::writeImage(fname.str(), projection, ImageSerializer::TIFF_GRAY_32);

                boost::filesystem::path filepath = fname.str() + ".tif";
                std::string relativeNameToImageFile = boost::filesystem::basename(filepath) + boost::filesystem::extension(filepath);
                filenames.push_back(relativeNameToImageFile);
                tiltAngles.push_back(tiltPosition);
            }
            ImageStackDirectoryDatasource::writeMetaDataFile(filenameTrunk + "/metadata.xml", tiltAngles, filenames, satRotator.GetBaseScannerGeometry(), geometricSetup->getVolume()->Properties());

            delete traversalLength;
            delete projection;
            delete forwardKernel;
        }

        std::string compute_Synthetic_3_Parallel(ParameterSource* xmlParameter)
        {
            xmlParameter->LogLevel(ParameterSource::LogLevelValues::SILENT);
            framework->parseAndAddParameterSource(xmlParameter);

            auto app = new BlockIterativeReconstructionOperator(framework);
            app->run();
            framework->writeFinalVolume(app->getReconstructedVolume());
            std::string ret = framework->getParameterSet()->get<IOParameterSet>()->OutputVolumeFileName().string();
            delete app;
            return ret;
        }

        GPUMappedVolume* mappedVolume;
        CLMemBuffer* residualBuffer;
        AlgebraicParameterSet* parameterSet;
    };
}

using namespace ettention;

TEST_F(SyntheticReconstructionTest, DISABLED_ParallelBeamsSyntheticDataGeneration) 
{
    generateSyntheticDatasetUsingParallelForwardProjection(std::string(TESTDATA_DIR) + "/data/synthetic_3_parallel");
}  

TEST_F(SyntheticReconstructionTest, SART_Parallel) 
{
    std::string testDataDirectory(std::string(TESTDATA_DIR) + "/data/synthetic_3_parallel");
    ParameterSource* xmlParameter = new XMLParameterSource(testDataDirectory + "/sart.xml");

    std::string computedStack = compute_Synthetic_3_Parallel(xmlParameter);

    const float rms = StackComparator::getRMSBetweenVolumes(computedStack, testDataDirectory + "/ground_truth.mrc");

    delete xmlParameter;

    std::cout << "RMS (SART_Parallel) " << rms << std::endl;

    ASSERT_TRUE(rms <= 0.7f);
} 

TEST_F(SyntheticReconstructionTest, SIRT_Parallel)
{
    std::string testDataDirectory(std::string(TESTDATA_DIR) + "/data/synthetic_3_parallel");
    ParameterSource* xmlParameter = new XMLParameterSource(testDataDirectory + "/sirt.xml");

    std::string computedStack = compute_Synthetic_3_Parallel(xmlParameter);

    const float rms = StackComparator::getRMSBetweenVolumes(computedStack, testDataDirectory + "/ground_truth.mrc");

    delete xmlParameter;
    std::cout << "RMS (SIRT_Parallel) " << rms << std::endl;

    ASSERT_TRUE(rms <= 0.7f);
}

TEST_F(SyntheticReconstructionTest, BLOCKITERATIVE_Parallel)
{
    std::string testDataDirectory(std::string(TESTDATA_DIR) + "/data/synthetic_3_parallel");
    ParameterSource* xmlParameter = new XMLParameterSource(testDataDirectory + "/blockIterative.xml");

    std::string computedStack = compute_Synthetic_3_Parallel(xmlParameter);

    const float rms = StackComparator::getRMSBetweenVolumes(computedStack, testDataDirectory + "/ground_truth.mrc");

    delete xmlParameter;
    std::cout << "RMS (BLOCKITERATIVE_Parallel)" << rms << std::endl;

    ASSERT_TRUE(rms <= 0.2f);
}
