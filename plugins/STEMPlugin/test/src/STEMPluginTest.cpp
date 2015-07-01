#include "ExternalIncludes.h"
#include "gtest/gtest.h"
#include "framework/test/CLTestBase.h"
#include "framework/RandomAlgorithms.h"
#include "framework/plugins/PluginManager.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "model/volume/Volume.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/image/ImageComparator.h"
#include "model/volume/StackComparator.h"
#include "model/volume/VoxelType.h"
#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "reconstruction/projections/RegularizationBackprojectionKernel.h"
#include "reconstruction/projections/AdjointBackProjectionKernel.h"
#include "algorithm/longobjectcompensation/ComputeRayLengthKernel.h"
#include "reconstruction/GenerateCircularPSFKernel.h"
#include "reconstruction/projections/STEMForwardProjection.h"
#include "reconstruction/operators/PreFilterOperator.h"
#include "reconstruction/operators/Adjoint_BackProjection_Operator.h"
#include "reconstruction/operators/Regularization_BackProjection_Operator.h"
#include "reconstruction/operators/STEMForwardProjectionOperator.h"
#include "setup/TF_ART_ParameterSet.h"
#include "setup/parametersource/XMLParameterSource.h"
#include "io/TF_Datasource.h"
#include "reconstruction/TF_ART.h"
#include "STEMPlugin.h"

namespace ettention
{
    namespace stem
    {
        class STEMPluginTest : public CLTestBase
        {
        public:
            STEMPluginTest()
                : CLTestBase(HardwareParameterSet::nvidia, HardwareParameterSet::GPU_only)
                , volumeOptions(Vec3ui(128, 128, 128), 0.0f, BoundingBox3f(Vec3f(-64.0f, -64.0f, -64.0f), Vec3f(64.0f, 64.0f, 64.0f)))
            {
            }

            ~STEMPluginTest()
            {
            }

            virtual void SetUp()
            {
                CLTestBase::SetUp();
                testdataDirectory = std::string(TESTDATA_DIR) + "/data/";
                workDirectory = std::string(TESTDATA_DIR) + "/work/";

                projectionResolution = Vec2ui(256, 256);

                geometricSetup = GetDefaultGeometricSetup();
                geometricSetup->setVolume(new Volume(VolumeProperties(VoxelType::FLOAT_32, volumeOptions, 1), 0.0f));

                rayLengthImage = new GPUMapped<Image>(clal, projectionResolution);
                computeRayLength = new ComputeRayLengthKernel(framework, geometricSetup, rayLengthImage);

                plugin = new STEMPlugin();
                framework->getPluginManager()->registerPlugin(plugin);
            }

            virtual void TearDown()
            {
                delete plugin;
                delete computeRayLength;
                delete rayLengthImage;
                delete geometricSetup->getVolume();
                delete geometricSetup;
                CLTestBase::TearDown();
            }

            GeometricSetup* GetDefaultGeometricSetup()
            {
                sourcePosition = Vec3f(-64.0f, -64.0f, -128.0f);
                detectorPosition = Vec3f(-64.0f, -64.0f, 128.0f);
                Vec3f horizontalPitch(0.5f, 0.0f, 0.0f);
                Vec3f verticalPitch(0.0f, 0.5f, 0.0f);
                ScannerGeometry baseScannerGeometry(ScannerGeometry::PROJECTION_PARALLEL, projectionResolution);
                baseScannerGeometry.set(sourcePosition, detectorPosition, horizontalPitch, verticalPitch);
                satRotator.SetBaseScannerGeometry(baseScannerGeometry);
                return new GeometricSetup(baseScannerGeometry);
            }

            Vec2ui projectionResolution;
            SingleAxisTiltRotator satRotator;
            GeometricSetup* geometricSetup;

            std::shared_ptr<HardwareParameterSet> hwParameterSet;
            ComputeRayLengthKernel* computeRayLength;

            GPUMapped<Image>* residual;
            GPUMapped<Image>* rayLengthImage;
            std::string testdataDirectory, workDirectory;

            Vec3f sourcePosition, detectorPosition;
            STEMPlugin* plugin;
            PluginManager* pluginManager;
            VolumeParameterSet volumeOptions;
        };
    }
}

using namespace ettention;
using namespace stem;

TEST_F(STEMPluginTest, Adjoint_BackProjection_Constructor)
{
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());
    auto image = new Image(projectionResolution);
    residual = new GPUMapped<Image>(clal, image);
    AdjointBackProjectionKernel* backProjection = new AdjointBackProjectionKernel(framework, geometricSetup, residual, computeRayLength->getOutput(), mappedVolume->getMappedSubVolume(), 1.0f, false);
    delete backProjection;
    delete residual;
    delete image;
    delete mappedVolume;
}

TEST_F(STEMPluginTest, GenerateCircularPSFTest)
{
    GenerateCircularPSFKernel* kernel = new GenerateCircularPSFKernel(framework, Vec2ui(256, 256), 16.0f);
    kernel->run();

    ImageSerializer::writeImage(workDirectory + "adjoint/circle256", kernel->getOutput(), ImageSerializer::TIFF_GRAY_32);

    delete kernel;
    ImageComparator::assertImagesAreEqual(workDirectory + "adjoint/circle256.tif", testdataDirectory + "adjoint/circle256.tif");
}

TEST_F(STEMPluginTest, PreFilterResidualsTest)
{
    auto tempImage = ImageDeserializer::readImage(testdataDirectory + "adjoint/testdata.tif");
    GPUMapped<Image>* image = new GPUMapped<Image>(clal, tempImage);
    ImageSerializer::writeImage(workDirectory + "adjoint/testdata_output", image, ImageSerializer::TIFF_GRAY_32);

    PrefilterOperator* preFilter = new PrefilterOperator(framework, image, 10.0f);

    preFilter->run();
    ImageSerializer::writeImage(workDirectory + "adjoint/prefiltered_residuals", preFilter->getOutput(), ImageSerializer::TIFF_GRAY_32);

    delete preFilter;

    ImageComparator::assertImagesAreEqual(workDirectory + "adjoint/prefiltered_residuals.tif", testdataDirectory + "adjoint/prefiltered_residuals.tif");
    delete image;
    delete tempImage;
}

TEST_F(STEMPluginTest, DISABLED_PreFilterResidualsDetermineDeltaR)
{
    auto tempImage = ImageDeserializer::readImage(testdataDirectory + "adjoint/testdata_realworld.tif");
    GPUMapped<Image>* image = new GPUMapped<Image>(clal, tempImage);
    ImageSerializer::writeImage(workDirectory + "adjoint/testdata_output", image, ImageSerializer::TIFF_GRAY_32);

    PrefilterOperator* preFilter = new PrefilterOperator(framework, image, 10.0f);

    preFilter->run();

    preFilter->getOutput()->ensureIsUpToDateOnCPU();
    Image* preFilteredImage = preFilter->getOutput()->getObjectOnCPU();

    unsigned int nSamples = 100;
    std::ofstream ofs(workDirectory + "adjoint/determine_delta_r.csv", std::ios::out);
    ofs << "x;y;";
    for(unsigned int i = 0; i < nSamples; i++)
        ofs << "value" << i << ";";
    ofs << "totalError" << std::endl;

    std::vector<Vec2ui> samplePositions;
    RandomAlgorithms::generateRandomPointsFromZeroToMax(100, image->getResolution(), samplePositions);
    for(unsigned int i = 0; i < nSamples; i++)
    {
        Vec2ui samplePosition = samplePositions[i];
        ofs << samplePosition.x << ";" << samplePosition.y << ";";
        float totalError = 0.0f;
        for(unsigned int z = 1; z < preFilter->NumberOfSamplingSteps() - 1; z++)
        {
            float value0 = preFilteredImage->getPixel(samplePosition.x, samplePosition.y + (z - 1) * image->getResolution().y);
            float value1 = preFilteredImage->getPixel(samplePosition.x, samplePosition.y + (z)* image->getResolution().y);
            float value2 = preFilteredImage->getPixel(samplePosition.x, samplePosition.y + (z + 1) * image->getResolution().y);
            float theoreticValue = (value2 + value0) / 2.0f;
            float error = fabs(value1 - theoreticValue);
            ofs << value1 << ";";
            totalError += error;
        }
        ofs << totalError << std::endl;
    }
    ofs.close();

    ImageSerializer::writeImage(workDirectory + "adjoint/prefiltered_residuals_determine_delta_r", preFilter->getOutput(), ImageSerializer::TIFF_GRAY_32);

    delete preFilter;

    ImageComparator::assertImagesAreEqual(workDirectory + "adjoint/prefiltered_residuals.tif", testdataDirectory + "adjoint/prefiltered_residuals.tif");
    delete image;
    delete tempImage;
}

TEST_F(STEMPluginTest, DISABLED_AdjointBackProjectionKernelZ_Datageneration)
{
    GPUMapped<Image>* image = new GPUMapped<Image>(clal, ImageDeserializer::readImage(testdataDirectory + "adjoint/projection_z.tif"));
    ImageSerializer::writeImage(workDirectory + "adjoint/testdata_output", image, ImageSerializer::TIFF_GRAY_32);

    PrefilterOperator* preFilter = new PrefilterOperator(framework, image, 20.0f);

    preFilter->run();
    ImageSerializer::writeImage(workDirectory + "adjoint/prefiltered_residual_z", preFilter->getOutput(), ImageSerializer::TIFF_GRAY_32);

    delete preFilter;

    ImageComparator::assertImagesAreEqual(workDirectory + "adjoint/prefiltered_residuals.tif", testdataDirectory + "adjoint/prefiltered_residuals.tif");
    delete image;
}


TEST_F(STEMPluginTest, AdjointBackProjectionKernelZ)
{
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());
    auto image = ImageDeserializer::readImage(testdataDirectory + "adjoint/prefiltered_residual_z.tif");
    GPUMapped<Image>* prefilteredResidual = new GPUMapped<Image>(clal, image);
    TFProjectionProperties projectionProps(0.0f, 0.0f, 16.0f, 0.21f);

    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));
    prefilteredResidual->ensureIsUpToDateOnGPU();

    AdjointBackProjectionKernel* backProjection = new AdjointBackProjectionKernel(framework, geometricSetup, prefilteredResidual, computeRayLength->getOutput(), mappedVolume->getMappedSubVolume(), 1.0f, false);
    backProjection->setProjectionProperties(projectionProps);

    computeRayLength->run();

    backProjection->run();

    MRCWriter writer;
    mappedVolume->ensureIsUpToDateOnCPU();
    const OutputFormatParameterSet format;
    writer.write(mappedVolume->getObjectOnCPU(), workDirectory + "adjoint/adjoint_generated_z.mrc", &format);

    delete backProjection;
    delete prefilteredResidual;
    delete image;
    delete mappedVolume;

    StackComparator::assertVolumesAreEqual(testdataDirectory + "adjoint/adjoint_reference_z.mrc", workDirectory + "adjoint/adjoint_generated_z.mrc");
}

TEST_F(STEMPluginTest, AdjointBackProjectionOperatorZ_OneSubvolume)
{
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());
    GPUMapped<Image>* residual = new GPUMapped<Image>(clal, ImageDeserializer::readImage(testdataDirectory + "adjoint/projection_z.tif"));
    residual->takeOwnershipOfObjectOnCPU();
    TFProjectionProperties projectionProps(0.0f, 0.0f, 16.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));

    Adjoint_BackProjectionOperator* adjointOperator = new Adjoint_BackProjectionOperator(framework, geometricSetup, mappedVolume, residual, 1.0f);
    adjointOperator->SetProjectionProperties(projectionProps);
    adjointOperator->run();

    MRCWriter writer;
    mappedVolume->ensureIsUpToDateOnCPU();
    const OutputFormatParameterSet format;
    writer.write(mappedVolume->getObjectOnCPU(), workDirectory + "adjoint/adjoint_operator_generated_z.mrc", &format);

    delete adjointOperator;
    delete residual;
    delete mappedVolume;

    StackComparator::assertVolumesAreEqual(testdataDirectory + "adjoint/adjoint_reference_z.mrc", workDirectory + "adjoint/adjoint_operator_generated_z.mrc");
}

TEST_F(STEMPluginTest, AdjointBackProjectionOperatorZ_TwoSubvolume)
{
    geometricSetup->getVolume()->Properties().SetSubVolumeCount(2);
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());

    GPUMapped<Image>* residual = new GPUMapped<Image>(clal, ImageDeserializer::readImage(testdataDirectory + "adjoint/projection_z.tif"));
    residual->takeOwnershipOfObjectOnCPU();
    TFProjectionProperties projectionProps(0.0f, 0.0f, 16.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));

    Adjoint_BackProjectionOperator* adjointOperator = new Adjoint_BackProjectionOperator(framework, geometricSetup, mappedVolume, residual, 1.0f);
    adjointOperator->SetProjectionProperties(projectionProps);
    adjointOperator->run();

    MRCWriter writer;
    mappedVolume->ensureIsUpToDateOnCPU();
    const OutputFormatParameterSet format;
    writer.write(mappedVolume->getObjectOnCPU(), workDirectory + "adjoint/adjoint_operator_generated_z_2_subvolumes.mrc", &format);

    delete adjointOperator;
    delete residual;
    delete mappedVolume;
    StackComparator::assertVolumesAreEqual(testdataDirectory + "adjoint/adjoint_reference_z.mrc", workDirectory + "adjoint/adjoint_operator_generated_z_2_subvolumes.mrc");
}

TEST_F(STEMPluginTest, RegularizationBackProjectionKernelZ)
{
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());
    GPUMapped<Image>* residual = new GPUMapped<Image>(clal, ImageDeserializer::readImage(testdataDirectory + "regularization/projection_z.tif"));
    residual->takeOwnershipOfObjectOnCPU();
    TFProjectionProperties projectionProps(0.0f, 0.0f, 16.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));

    RegularizationBackProjectionKernel* backProjection = new RegularizationBackProjectionKernel(framework, geometricSetup, residual, computeRayLength->getOutput(), mappedVolume->getMappedSubVolume(), 1.0f, false);
    backProjection->setProjectionProperties(projectionProps);

    computeRayLength->run();

    backProjection->run();

    MRCWriter writer;
    mappedVolume->ensureIsUpToDateOnCPU();
    const OutputFormatParameterSet format;
    writer.write(mappedVolume->getObjectOnCPU(), workDirectory + "regularization/regularization_generated_z.mrc", &format);

    delete backProjection;
    delete residual;
    delete mappedVolume;

    StackComparator::assertVolumesAreEqual(testdataDirectory + "regularization/regularization_reference_z.mrc", workDirectory + "regularization/regularization_generated_z.mrc");
}

TEST_F(STEMPluginTest, RegularizationBackProjectionOperatorZ_OneSubvolume)
{   
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());
    residual = new GPUMapped<Image>(clal, ImageDeserializer::readImage(testdataDirectory + "regularization/projection_z.tif"));
    residual->takeOwnershipOfObjectOnCPU();
    TFProjectionProperties projectionProps(0.0f, 128.0f, 16.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));

    Regularization_BackProjectionOperator* regularizationOperator = new Regularization_BackProjectionOperator(framework, geometricSetup, mappedVolume, residual, 1.0f);
    regularizationOperator->SetProjectionProperties(projectionProps);
    regularizationOperator->run();
    delete regularizationOperator;

    MRCWriter writer;
    mappedVolume->ensureIsUpToDateOnCPU();
    mappedVolume->ensureIsUpToDateOnCPU();
    const OutputFormatParameterSet format;
    writer.write(mappedVolume->getObjectOnCPU(), workDirectory + "regularization/regularization_operator_generated_z.mrc", &format);

    delete residual;
    delete mappedVolume;
}

TEST_F(STEMPluginTest, STEM_Forward_Projection_Kernel)
{
    delete geometricSetup->getVolume();
    geometricSetup->setVolume(VolumeDeserializer::load(testdataDirectory + "tf_art/particles_3.mrc", VoxelType::FLOAT_32));
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());

    STEMForwardProjectionKernel* forwardKernel = new STEMForwardProjectionKernel(framework, projectionResolution, geometricSetup, satRotator.GetBaseScannerGeometry().getSourceBase(), mappedVolume, 128);

    TFProjectionProperties projectionProps(10.0f, 0.0f, 64.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));

    forwardKernel->setProjectionProperties(projectionProps);
    forwardKernel->run();

    forwardKernel->getOutput()->ensureIsUpToDateOnCPU();
    ImageSerializer::writeImage(workDirectory + "forward/projection", forwardKernel->getOutput(), ImageSerializer::TIFF_GRAY_32);

    ImageComparator::assertImagesAreEqual(workDirectory + "forward/projection.tif", testdataDirectory + "forward/projection_reference.tif");

    delete forwardKernel;
    delete mappedVolume;
}

TEST_F(STEMPluginTest, STEM_Forward_Projection_FocalDistance)
{
    delete geometricSetup->getVolume();
    geometricSetup->setVolume(VolumeDeserializer::load(testdataDirectory + "tf_art/particles_3.mrc", VoxelType::FLOAT_32));
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());

    STEMForwardProjectionKernel* forwardKernel = new STEMForwardProjectionKernel(framework, projectionResolution, geometricSetup, satRotator.GetBaseScannerGeometry().getSourceBase(), mappedVolume, 128);

    TFProjectionProperties projectionProps(10.0f, 0.0f, 64.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));
    forwardKernel->setProjectionProperties(projectionProps);
    forwardKernel->getOutput()->getBufferOnGPU()->clear();
    forwardKernel->getTraversalLength()->getBufferOnGPU()->clear();
    forwardKernel->run();
    ImageSerializer::writeImage(workDirectory + "forward/projection_focus_0", forwardKernel->getOutput(), ImageSerializer::TIFF_GRAY_32);
    ImageComparator::assertImagesAreEqual(workDirectory + "forward/projection_focus_0.tif", testdataDirectory + "forward/projection_reference.tif");

    projectionProps = TFProjectionProperties(0.0f, 32.0f, 64.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));
    forwardKernel->setProjectionProperties(projectionProps);
    forwardKernel->getOutput()->getBufferOnGPU()->clear();
    forwardKernel->getTraversalLength()->getBufferOnGPU()->clear();
    forwardKernel->run();
    ImageSerializer::writeImage(workDirectory + "forward/projection_focus_32", forwardKernel->getOutput(), ImageSerializer::TIFF_GRAY_32);
    ImageComparator::assertImagesAreEqual(workDirectory + "forward/projection_focus_32.tif", testdataDirectory + "forward/projection_reference_32.tif");

    delete forwardKernel;
    delete mappedVolume;
}

TEST_F(STEMPluginTest, STEM_Forward_Projection_Operator_One_SubVolumes)
{
    delete geometricSetup->getVolume();
    geometricSetup->setVolume(VolumeDeserializer::load(testdataDirectory + "tf_art/particles_3.mrc", VoxelType::FLOAT_32));
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());

    STEMForwardProjectionOperator* forwardOperator = new STEMForwardProjectionOperator(framework, projectionResolution, geometricSetup, satRotator.GetBaseScannerGeometry().getSourceBase(), mappedVolume, 128);

    TFProjectionProperties projectionProps(10.0f, 0.0f, 64.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));
    forwardOperator->SetProjectionProperties(projectionProps);
    forwardOperator->run();

    forwardOperator->getOutput()->ensureIsUpToDateOnCPU();
    ImageSerializer::writeImage(workDirectory + "forward/projection_1_subvolumes", forwardOperator->getOutput(), ImageSerializer::TIFF_GRAY_32);

    ImageComparator::assertImagesAreEqual(workDirectory + "forward/projection_1_subvolumes.tif", testdataDirectory + "forward/projection_reference.tif");

    delete forwardOperator;
    delete mappedVolume;
}

TEST_F(STEMPluginTest, STEM_Forward_Projection_Operator_Two_SubVolumes)
{
    delete geometricSetup->getVolume();
    geometricSetup->setVolume(VolumeDeserializer::load(testdataDirectory + "tf_art/particles_3.mrc", VoxelType::FLOAT_32));
    geometricSetup->getVolume()->Properties().SetSubVolumeCount(2);
    auto mappedVolume = new GPUMappedVolume(clal, geometricSetup->getVolume());

    STEMForwardProjectionOperator* forwardOperator = new STEMForwardProjectionOperator(framework, projectionResolution, geometricSetup, satRotator.GetBaseScannerGeometry().getSourceBase(), mappedVolume, 128);

    TFProjectionProperties projectionProps(10.0f, 0.0f, 64.0f, 0.21f);
    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(projectionProps.GetTiltAngle()));

    forwardOperator->SetProjectionProperties(projectionProps);
    forwardOperator->run();

    forwardOperator->getOutput()->ensureIsUpToDateOnCPU();
    ImageSerializer::writeImage(workDirectory + "forward/projection_2_subvolumes", forwardOperator->getOutput(), ImageSerializer::TIFF_GRAY_32);

    ImageComparator::assertImagesAreEqual(workDirectory + "forward/projection_2_subvolumes.tif", testdataDirectory + "forward/projection_reference.tif");

    delete forwardOperator;
    delete mappedVolume;
}

TEST_F(STEMPluginTest, TF_ART_TEST_Regularization)
{
    delete geometricSetup->getVolume();
    geometricSetup->setVolume(new Volume(VolumeProperties(VoxelType::FLOAT_32, volumeOptions, 1), 1.0f));
    framework->getImageStackDataSourceFactory()->addDataSourcePrototype(20, new TF_Datasource());

    XMLParameterSource* xmlParameter = new XMLParameterSource(testdataDirectory + "/tf_art/regularization.xml");
    framework->parseAndAddParameterSource(xmlParameter);

    TF_ART application(framework);
    application.run();
    delete xmlParameter;
}

TEST_F(STEMPluginTest, TF_ART_TEST_Adjoint)
{
    delete geometricSetup->getVolume();
    geometricSetup->setVolume(new Volume(VolumeProperties(VoxelType::FLOAT_32, volumeOptions, 1), 1.0f));
    framework->getImageStackDataSourceFactory()->addDataSourcePrototype(20, new TF_Datasource());

    XMLParameterSource* xmlParameter = new XMLParameterSource(testdataDirectory + "/tf_art/adjoint.xml");
    framework->parseAndAddParameterSource(xmlParameter);

    TF_ART application(framework);
    application.run();
    delete xmlParameter;
}