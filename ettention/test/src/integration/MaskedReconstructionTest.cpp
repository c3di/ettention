#include "stdafx.h"
#include "gtest/gtest.h"

#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/reconstruction/ReconstructionAlgorithm.h"
#include "framework/Framework.h"
#include "framework/test/ProjectionTestBase.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "model/projectionset/factory/ProjectionIteratorFactory.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/InputParameterSet.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/parameterset/PriorKnowledgeParameterSet.h"
#include "setup/parametersource/XMLParameterSource.h"

namespace ettention
{
    class MaskedReconstructionTestLogger
    {
    public:
        MaskedReconstructionTestLogger()
        {
            Logger::getInstance().setLogFilename(std::string(TESTDATA_DIR) + "/work/masked_reconstruction/contour.log");
            Logger::getInstance().activateFileLog(Logger::FORMAT_WIDE);
        }

        ~MaskedReconstructionTestLogger()
        {
            Logger::getInstance().forceLogFlush();
            Logger::getInstance().deactivateFileLog();
        }
    };
    static MaskedReconstructionTestLogger localLogger;

    class MaskedReconstructionTest : public ProjectionTestBase
    {
    public:
        MaskedReconstructionTest() : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/masked_reconstruction/", std::string(TESTDATA_DIR) + "/work/masked_reconstruction/")
        {
        }

        ~MaskedReconstructionTest(void)
        {
        }

        virtual void SetUp() override
        {
            ProjectionTestBase::SetUp();
            generateProjections();
        }

        virtual void allocateVolume() override
        {
            delete volume;
            volume = VolumeDeserializer::load(testdataDirectory + "shepp_logan.mrc", Voxel::DataType::FLOAT_32);
        }

        std::string doReconstruct(std::string xmlConfigPath)
        {
            XMLParameterSource xmlParameterSource(testdataDirectory + xmlConfigPath);
            framework->resetParameterSource(&xmlParameterSource);

            auto reconstructionAlgorithm = framework->instantiateReconstructionAlgorithm();
            reconstructionAlgorithm->run();
            framework->writeFinalVolume(reconstructionAlgorithm->getReconstructedVolume());
            delete reconstructionAlgorithm;

            return framework->getParameterSet()->get<OutputParameterSet>()->getFilename().string();
        }

        bool checkIfOnlyMaskRegionWasUpdated(std::string xmlConfigPath)
        {
            XMLParameterSource xmlParameterSource(testdataDirectory + xmlConfigPath);
            framework->resetParameterSource(&xmlParameterSource);
            auto reconstructionAlgorithm = framework->instantiateReconstructionAlgorithm();
            reconstructionAlgorithm->run();

            auto maskName = framework->getParameterSet()->get<PriorKnowledgeParameterSet>()->getMaskVolumeFilename();
            auto maskVolume = VolumeDeserializer::load(maskName.string(), Voxel::DataType::UCHAR_8);

            auto resultVolume = reconstructionAlgorithm->getReconstructedVolume();
            Vec3ui resolution = resultVolume->getProperties().getVolumeResolution();

            float tempMask;
            float tempResult;
            float summ = 0.0f;
            Vec3i pos;
            for( pos.z = 0; pos.z < (int) resolution.z; ++pos.z )
            {
                for( pos.y = 0; pos.y < (int)resolution.y; ++pos.y )
                {
                    for( pos.x = 0; pos.x < (int)resolution.x; ++pos.x )
                    {
                        tempMask    = maskVolume->getVoxelValue(pos);
                        tempResult  = resultVolume->getObjectOnCPU()->getVoxelValue(pos);

                        float diff  = fabs(tempResult);
                        summ += diff * (!tempMask);
                    }
                }
            }

            delete maskVolume;
            delete reconstructionAlgorithm;

            return (summ < 0.0000001f);
        }

        void generateProjections()
        {
            if( testDataGenerated )
                return;

            if( boost::filesystem::exists(workDirectory + "shepp_logan_projections/metadata.xml") )
                return;

            const Vec2ui resolution(256, 256);
            auto sheppLogan = VolumeDeserializer::load(testdataDirectory + "shepp_logan.mrc", Voxel::DataType::FLOAT_32);
            auto gpuMappedSheppLogan = new GPUMappedVolume(framework->getOpenCLStack(), sheppLogan);
            geometricSetup->setVolume(sheppLogan);
            auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), resolution);
            auto rayLength = new GPUMapped<Image>(framework->getOpenCLStack(), resolution);
            auto projectionKernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, gpuMappedSheppLogan, nullptr, 5);
            projectionKernel->setOutput(projection, rayLength);
                    
            std::ofstream metadata(workDirectory + "shepp_logan_projections/metadata.xml");
            metadata << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
            metadata << "<geometry>" << std::endl;
            metadata << "    <detectorPosition value=\"" << satRotator->getBaseScannerGeometry()->getDetectorBase().x << " " << satRotator->getBaseScannerGeometry()->getDetectorBase().y << " " << satRotator->getBaseScannerGeometry()->getDetectorBase().z << "\" />" << std::endl;
            metadata << "    <sourcePosition value=\"" << satRotator->getBaseScannerGeometry()->getSourceBase().x << " " << satRotator->getBaseScannerGeometry()->getSourceBase().y << " " << satRotator->getBaseScannerGeometry()->getSourceBase().z << "\" />" << std::endl;
            metadata << "    <horizontalPitch value=\"" << satRotator->getBaseScannerGeometry()->getHorizontalPitch().x << " " << satRotator->getBaseScannerGeometry()->getHorizontalPitch().y << " " << satRotator->getBaseScannerGeometry()->getHorizontalPitch().z << "\" />" << std::endl;
            metadata << "    <verticalPitch value=\"" << satRotator->getBaseScannerGeometry()->getVerticalPitch().x << " " << satRotator->getBaseScannerGeometry()->getVerticalPitch().y << " " << satRotator->getBaseScannerGeometry()->getVerticalPitch().z << "\" />" << std::endl;
            metadata << "</geometry>" << std::endl;

            int startingAngle = -90;
            int endAngle = 90;
            unsigned int projectionCount = 91;
            for( unsigned int i = 0; i < projectionCount; ++i )
            {
                float tiltAngle = (float)startingAngle + (float)i / (float)(projectionCount - 1) * (float)(endAngle - startingAngle);
                geometricSetup->setScannerGeometry(satRotator->createRotatedScannerGeometry(tiltAngle));
                projectionKernel->run();
                std::stringstream filename;
                filename << "projection_" << i;
                ImageSerializer::writeImage(workDirectory + "shepp_logan_projections/" + filename.str(), projectionKernel->getVirtualProjection(), ImageSerializer::TIFF_GRAY_32);
                projectionKernel->getVirtualProjection()->getBufferOnGPU()->clear();
                metadata << "<projection>" << std::endl;
                metadata << "    <image filename=\"" << filename.str() << ".tif\" />" << std::endl;
                metadata << "    <tiltAngle value=\"" << tiltAngle << "\" />" << std::endl;
                metadata << "</projection>" << std::endl;
            }
            metadata.close();
            geometricSetup->setScannerGeometry(satRotator->getBaseScannerGeometry()->clone());

            delete projectionKernel;
            delete rayLength;
            delete projection;
            delete gpuMappedSheppLogan;
            delete sheppLogan;

            testDataGenerated = true;
        }

        GPUMappedVolume* mappedVolume;
        GPUMappedVolume* priorKnowledgeMask;

        bool testDataGenerated = false;
    };
}

using namespace ettention;

TEST_F(MaskedReconstructionTest, ForwardProjectionKernelTest)
{
    mappedVolume = new GPUMappedVolume(framework->getOpenCLStack(), volume);
    mappedVolume->ensureIsUpToDateOnGPU();
    
    Volume* priorKnowledgeVolume = VolumeDeserializer::load(testdataDirectory + "mask.mrc", Voxel::DataType::UCHAR_8);
    geometricSetup->setVolume(priorKnowledgeVolume);

    priorKnowledgeMask = new GPUMappedVolume(framework->getOpenCLStack(), priorKnowledgeVolume);

    ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, priorKnowledgeMask, 0);

    GPUMapped<Image>* projection = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
    GPUMapped<Image>* traversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
    kernel->setOutput(projection, traversalLength);

    Logger::getInstance().activateConsoleLog(Logger::FORMAT_WIDE);
    framework->getOpenCLStack()->printCurrentDeviceName();
    Logger::getInstance().deactivateConsoleLog();
    Logger::getInstance().forceLogFlush();
    
    geometricSetup->setScannerGeometry(satRotator->createRotatedScannerGeometry(45.0f));
    kernel->run();

    ImageSerializer::writeImage(workDirectory + "generated", projection, ImageSerializer::ImageFormat::TIFF_GRAY_32);
    ImageSerializer::writeImage(workDirectory + "raylength", traversalLength, ImageSerializer::ImageFormat::TIFF_GRAY_32);

    ImageComparator::assertImagesAreEqual(workDirectory + "generated.tif", testdataDirectory + "generated.tif");
    ImageComparator::assertImagesAreEqual(workDirectory + "raylength.tif", testdataDirectory + "raylength.tif");

    delete priorKnowledgeMask;
    delete priorKnowledgeVolume;

    delete traversalLength;
    delete projection;

    delete kernel;
    delete mappedVolume;
}

TEST_F(MaskedReconstructionTest, BackProjectionKernelTest)
{
    delete volume;
    volume = VolumeDeserializer::load(testdataDirectory + "empty.mrc", Voxel::DataType::FLOAT_32);
    geometricSetup->setVolume(volume);

    mappedVolume = new GPUMappedVolume(framework->getOpenCLStack(), volume);
    mappedVolume->ensureIsUpToDateOnGPU();

    Volume* priorKnowledgeVolume = VolumeDeserializer::load(testdataDirectory + "mask.mrc", Voxel::DataType::UCHAR_8);
    priorKnowledgeMask = new GPUMappedVolume(framework->getOpenCLStack(), priorKnowledgeVolume);

    ParallelBeamsBackProjectionKernel* kernel = new ParallelBeamsBackProjectionKernel(framework, geometricSetup, mappedVolume, priorKnowledgeMask, 0.3f, 1, false);

    Image* projectionImage = ImageDeserializer::readImage(testdataDirectory + "generated.tif");
    Image* rayLengthImageImage = ImageDeserializer::readImage(testdataDirectory + "raylength.tif");

    GPUMapped<Image>* projection = new GPUMapped<Image>(framework->getOpenCLStack(), projectionImage);
    GPUMapped<Image>* traversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), rayLengthImageImage);

    kernel->SetInput(projection, traversalLength);

    geometricSetup->setScannerGeometry(satRotator->createRotatedScannerGeometry(45.0f));
    kernel->run();

    mappedVolume->ensureIsUpToDateOnCPU();
    MRCWriter writer;
    const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XZY);
    writer.write(volume, workDirectory + "result.mrc", &format);

    delete mappedVolume; 

    delete priorKnowledgeMask;
    delete priorKnowledgeVolume;

    delete projectionImage;
    delete rayLengthImageImage;

    delete traversalLength;
    delete projection;

    delete kernel;
}

TEST_F(MaskedReconstructionTest, ChangeOnlyMaskedPartOfVolume)
{
    ASSERT_EQ(checkIfOnlyMaskRegionWasUpdated("integration_test_mask_only.xml"), true);
}

TEST_F(MaskedReconstructionTest, DISABLED_DifferentConfigurationTest) // Sviat: disabled on Jenkins till refactor
{
    std::string groundTruthPath = testdataDirectory + "shepp_logan.mrc";

    auto nonMaskedOutput            = doReconstruct("integration_test.xml");
    auto maskedOutput               = doReconstruct("integration_test_mask.xml");
    auto maskedDilatedOutput        = doReconstruct("integration_test_mask_dilated.xml");
    auto maskedAAOutput             = doReconstruct("integration_test_mask_antialiased.xml");
    auto maskedAADOutput            = doReconstruct("integration_test_mask_antialiased_dilated.xml");
    auto maskedAADOSOutput          = doReconstruct("integration_test_mask_antialiased_dilated_sample4.xml");

    auto nonMaskedRMS                                   = StackComparator::getRMSBetweenVolumes(nonMaskedOutput, groundTruthPath);
    auto maskedRMS                                      = StackComparator::getRMSBetweenVolumes(maskedOutput, groundTruthPath);
    auto maskedDilatedRMS                               = StackComparator::getRMSBetweenVolumes(maskedDilatedOutput, groundTruthPath);
    auto maskedAARMS                                    = StackComparator::getRMSBetweenVolumes(maskedAAOutput, groundTruthPath);
    auto maskedAADRMS                                   = StackComparator::getRMSBetweenVolumes(maskedAADOutput, groundTruthPath);
    auto maskedAADOSRMS                                 = StackComparator::getRMSBetweenVolumes(maskedAADOSOutput, groundTruthPath);
    auto diffBetweenMaskedAndNonMaskedReconstruction    = StackComparator::getRMSBetweenVolumes(nonMaskedOutput, maskedOutput);
    auto diffBetweenMaskedWithAndWithoutDilation        = StackComparator::getRMSBetweenVolumes(maskedOutput, maskedDilatedOutput);
    auto diffBetweenMaskedDilatedAndNonMaskedReconstruction = StackComparator::getRMSBetweenVolumes(nonMaskedOutput, maskedDilatedOutput);

    LOGGER("Full stack");
    LOGGER("Differences between:");

    LOGGER("  non-masked            reconstruction and phantom: " << nonMaskedRMS);
    LOGGER("                 masked reconstruction and phantom: " << maskedRMS);
    LOGGER("         dilated masked reconstruction and phantom: " << maskedDilatedRMS);
    LOGGER("     antialiased masked reconstruction and phantom: " << maskedAARMS);
    LOGGER("dilated and antialiased reconstruction and phantom: " << maskedAADRMS);
    LOGGER("dilated and antialiased oversample rec and phantom: " << maskedAADOSRMS);
    LOGGER("  non-masked and masked reconstructions           : " << diffBetweenMaskedAndNonMaskedReconstruction);
    LOGGER("          normal masked reconstruction and dilated: " << diffBetweenMaskedWithAndWithoutDilation);
    LOGGER("  non-masked and masked with dilation reconstructs: " << diffBetweenMaskedDilatedAndNonMaskedReconstruction);

    ASSERT_NEAR(0.0f, nonMaskedRMS, 7e-2f);
    ASSERT_NEAR(0.0f, maskedRMS, 7e-2f);
}

TEST_F(MaskedReconstructionTest, DISABLED_DifferentConfigurationWithMissingWedgeTest) // Sviat: disabled on Jenkins till we fix skipping projections mechanism to not duplicate input stacks.
{
    std::string groundTruthPath = testdataDirectory + "shepp_logan.mrc";

    auto nonMaskedOutput        = doReconstruct("missingwedge_test.xml");
    auto maskedOutput           = doReconstruct("missingwedge_test_mask.xml");
    auto maskedDilatedOutput    = doReconstruct("missingwedge_test_mask_dilated.xml");
    auto maskedAAOutput         = doReconstruct("missingwedge_test_mask_antialiased.xml");
    auto maskedAADOutput        = doReconstruct("missingwedge_test_mask_antialiased_dilated.xml");
    auto maskedAADOSOutput      = doReconstruct("missingwedge_test_mask_antialiased_dilated_sample4.xml");

    auto nonMaskedRMS                                   = StackComparator::getRMSBetweenVolumes(nonMaskedOutput, groundTruthPath);
    auto maskedRMS                                      = StackComparator::getRMSBetweenVolumes(maskedOutput, groundTruthPath);
    auto maskedDilatedRMS                               = StackComparator::getRMSBetweenVolumes(maskedDilatedOutput, groundTruthPath);
    auto maskedAARMS                                    = StackComparator::getRMSBetweenVolumes(maskedAAOutput, groundTruthPath);
    auto maskedAADRMS                                   = StackComparator::getRMSBetweenVolumes(maskedAADOutput, groundTruthPath);
    auto maskedAADOSRMS                                 = StackComparator::getRMSBetweenVolumes(maskedAADOSOutput, groundTruthPath);
    auto diffBetweenMaskedAndNonMaskedReconstruction    = StackComparator::getRMSBetweenVolumes(nonMaskedOutput, maskedOutput);
    auto diffBetweenMaskedWithAndWithoutDilation        = StackComparator::getRMSBetweenVolumes(maskedOutput, maskedDilatedOutput);
    auto diffBetweenMaskedDilatedAndNonMaskedReconstruction = StackComparator::getRMSBetweenVolumes(nonMaskedOutput, maskedDilatedOutput);

    LOGGER("Missing wedge stack");
    LOGGER("Differences between:");

    LOGGER("  non-masked            reconstruction and phantom: " << nonMaskedRMS);
    LOGGER("                 masked reconstruction and phantom: " << maskedRMS);
    LOGGER("         dilated masked reconstruction and phantom: " << maskedDilatedRMS);
    LOGGER("     antialiased masked reconstruction and phantom: " << maskedAARMS);
    LOGGER("dilated and antialiased reconstruction and phantom: " << maskedAADRMS);
    LOGGER("dilated and antialiased oversample rec and phantom: " << maskedAADOSRMS);
    LOGGER("  non-masked and masked reconstructions           : " << diffBetweenMaskedAndNonMaskedReconstruction);
    LOGGER("          normal masked reconstruction and dilated: " << diffBetweenMaskedWithAndWithoutDilation);
    LOGGER("  non-masked and masked with dilation reconstructs: " << diffBetweenMaskedDilatedAndNonMaskedReconstruction);
}