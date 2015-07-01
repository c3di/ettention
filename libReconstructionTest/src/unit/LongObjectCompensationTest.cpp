#include "stdafx.h"
#include "algorithm/longobjectcompensation/VirtualProjectionKernel.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "io/serializer/ImageSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "framework/test/ProjectionTestBase.h"
#include "model/geometry/GeometricSetup.h"
#include "model/image/ImageComparator.h"

namespace ettention
{
    class LongObjectCompensationTest : public ProjectionTestBase
    {
    public:
        LongObjectCompensationTest()
            : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/", std::string(TESTDATA_DIR) + "/work/")
        {
        }

        virtual void SetUp() override
        {
            CLTestBase::SetUp();
            Logger::getInstance();

            volumeResolution = Vec3ui(32, 32, 32);
            Vec3f volumeDimension = Vec3f((Vec2f)volumeResolution.xy(), 256.0f);
            volumeOptions = new VolumeParameterSet(volumeResolution, 0.0f, BoundingBox3f(-0.5f * volumeDimension, 0.5f * volumeDimension));

            allocateVolume();

            projectionResolution = Vec2ui(256, 256);
            unsigned int projectionSizeInBytes = projectionResolution.x * projectionResolution.y * sizeof(float);

            virtualProjection = new GPUMapped<Image>(clal, projectionResolution);
            traversalLength = new GPUMapped<Image>(clal, projectionResolution);

            tiltAngleInDegrees = 80.0f;
            tiltAngleInRadians = tiltAngleInDegrees / 180.0f * (float)M_PI;
            referenceConstantLength = (volumeDimension.z) / cosf(tiltAngleInRadians);

            geometricSetup = GetGeometricSetup();
        }

        virtual void TearDown() override
        {
            delete virtualProjection;
            delete traversalLength;

            ProjectionTestBase::TearDown();
        }

        virtual void allocateVolume() override
        {
            volume = new Volume(VolumeProperties(VoxelType::FLOAT_32, *volumeOptions, 1), 1.0f);
        }


        GeometricSetup* GetGeometricSetup()
        {
            Vec3f sourcePosition(-128.0f, -128.0f, -256.0f);
            Vec3f detectorPosition(-128.0f, -128.0f, 256.0f);
            Vec3f horizontalPitch(1.0f, 0.0f, 0.0f);
            Vec3f verticalPitch(0.0f, 1.0f, 0.0f);
            ScannerGeometry scannerGeometry(ScannerGeometry::PROJECTION_PARALLEL, projectionResolution);
            scannerGeometry.set(sourcePosition,
                                detectorPosition,
                                horizontalPitch,
                                verticalPitch);
            satRotator.SetBaseScannerGeometry(scannerGeometry);
            return new GeometricSetup(scannerGeometry);
        }

        void generateReferenceImage(std::string fileName, float value)
        {
            std::vector<float> imageBuffer;
            for(unsigned int i = 0; i < projectionResolution.x * projectionResolution.y; i++)
                imageBuffer.push_back(value);

            Image image(projectionResolution, &imageBuffer[0]);
            ImageSerializer::writeImage(fileName, &image, ImageSerializer::TIFF_GRAY_32);
        }

        GPUMapped<Image>* virtualProjection;
        GPUMapped<Image>* traversalLength;

        float tiltAngleInDegrees;
        float tiltAngleInRadians;
        float referenceConstantLength;
    };
}

using namespace ettention;

TEST_F(LongObjectCompensationTest, Unit_Long_Object_Compensation_Kernel_Constructor) 
{
    VirtualProjectionKernel* kernel = new VirtualProjectionKernel(framework, traversalLength);
    delete kernel;
}

TEST_F(LongObjectCompensationTest, DISABLED_Long_object_compensation_kernel_test_data_generation) 
{
    GPUMappedVolume* mappedVolume = new GPUMappedVolume(clal, volume);
    mappedVolume->ensureIsUpToDateOnGPU();

    ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, 16);
    auto projection = new GPUMapped<Image>(clal, projectionResolution);
    auto traversalLength = new GPUMapped<Image>(clal, projectionResolution);
    kernel->SetOutput(projection, traversalLength);    
    kernel->run();

    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/data/long_object_compensation/input_virtual_projection", projection, ImageSerializer::TIFF_GRAY_32);
    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/data/long_object_compensation/input_traversal_length", traversalLength, ImageSerializer::TIFF_GRAY_32);

    generateReferenceImage(std::string(TESTDATA_DIR) + "/data/long_object_compensation/virtual_projection_reference", referenceConstantLength);

    delete traversalLength;
    delete projection;
    delete kernel;

    delete mappedVolume;
}

TEST_F(LongObjectCompensationTest, Unit_Long_Object_Compensation) 
{
    Image* virtualProjectionImage = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/long_object_compensation/input_virtual_projection.tif");
    Image* traversalLengthImage = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/long_object_compensation/input_traversal_length.tif");

    virtualProjection->setObjectOnCPU(virtualProjectionImage);
    traversalLength->setObjectOnCPU(traversalLengthImage);

    VirtualProjectionKernel* kernel = new VirtualProjectionKernel(framework, traversalLength);

    geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry(tiltAngleInDegrees));

    float constantLength = geometricSetup->getScannerGeometry().getTraversalLengthThroughBoundingBox(volume->Properties().GetVolumeBoundingBox());

    ASSERT_TRUE(fabs(constantLength - referenceConstantLength) < 0.01f);

    kernel->setConstantLength(constantLength);
    kernel->setProjection(virtualProjection);
    kernel->run();

    ImageSerializer::writeImage(std::string(TESTDATA_DIR) + "/work/long_object_compensation/virtual_projection_generated", kernel->getProjection(), ImageSerializer::TIFF_GRAY_32);

    Image* generatedVirutalProjection = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/work/long_object_compensation/virtual_projection_generated.tif");
    Image* referenceVirtualProjection = ImageDeserializer::readImage(std::string(TESTDATA_DIR) + "/data/long_object_compensation/virtual_projection_reference.tif");

    ASSERT_TRUE(ImageComparator::areHeadersEqual(generatedVirutalProjection, referenceVirtualProjection));

    float error = ImageComparator::getRMS(generatedVirutalProjection, referenceVirtualProjection);

    ASSERT_TRUE(error < 0.1f);

    delete referenceVirtualProjection;
    delete generatedVirutalProjection;
    delete kernel;
    delete traversalLengthImage;
    delete virtualProjectionImage;
}