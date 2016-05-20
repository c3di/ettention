#include "stdafx.h"
#include "algorithm/longobjectcompensation/VirtualProjectionKernel.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "io/serializer/ImageSerializer.h"
#include "io/deserializer/ImageDeserializer.h"
#include "framework/test/ProjectionTestBase.h"
#include "model/geometry/GeometricSetup.h"
#include "model/geometry/ParallelScannerGeometry.h"
#include "model/image/ImageComparator.h"
#include "model/volume/FloatVolume.h"

using namespace ettention;

class LongObjectCompensationTest : public ProjectionTestBase
{
public:
    LongObjectCompensationTest()
        : ProjectionTestBase(std::string(TESTDATA_DIR) + "/data/long_object_compensation/", std::string(TESTDATA_DIR) + "/work/long_object_compensation/")
    {
    }

    virtual void SetUp() override
    {
        ProjectionTestBase::SetUp();
        Logger::getInstance();

        volumeResolution = Vec3ui(32, 32, 32);
        Vec3f volumeDimension = Vec3f(volumeResolution);
        delete volumeOptions;
        volumeOptions = new VolumeParameterSet(volumeResolution, 1.0f, BoundingBox3f(-0.5f * volumeDimension, 0.5f * volumeDimension));

        allocateVolume();

        projectionResolution = Vec2ui(256, 256);
        unsigned int projectionSizeInBytes = projectionResolution.x * projectionResolution.y * sizeof(float);

        virtualProjection = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        traversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);

        tiltAngleInDegrees = 80.0f;
        tiltAngleInRadians = tiltAngleInDegrees / 180.0f * (float) M_PI;
        referenceConstantLength = (volumeDimension.z) / cosf(tiltAngleInRadians);

        delete geometricSetup;
        geometricSetup = getGeometricSetup();
    }

    virtual void TearDown() override
    {
        delete traversalLength;
        delete virtualProjection;

        ProjectionTestBase::TearDown();
    }

    virtual void allocateVolume() override
    {
        delete volume;
        volume = new FloatVolume(volumeOptions);
    }


    GeometricSetup* getGeometricSetup()
    {
        Vec3f sourcePosition(-16.0f, -16.0f, -256.0f);
        Vec3f detectorPosition(-16.0f, -16.0f, 256.0f);
        Vec3f horizontalPitch(0.125f, 0.0f, 0.0f);
        Vec3f verticalPitch(0.0f, 0.125f, 0.0f);
        ScannerGeometry* scannerGeometry = new ParallelScannerGeometry( projectionResolution );
        scannerGeometry->set(sourcePosition,
                            detectorPosition,
                            horizontalPitch,
                            verticalPitch);
        satRotator->setBaseScannerGeometry(scannerGeometry);
        return new GeometricSetup(satRotator->createRotatedScannerGeometry(tiltAngleInDegrees));
    }

    void generateTestData()
    {
        GPUMappedVolume* mappedVolume = new GPUMappedVolume(framework->getOpenCLStack(), volume);
        mappedVolume->ensureIsUpToDateOnGPU();
        geometricSetup->setVolume(mappedVolume->getObjectOnCPU());

        ParallelBeamsForwardProjectionKernel* kernel = new ParallelBeamsForwardProjectionKernel(framework, geometricSetup, mappedVolume, nullptr, 16);
        auto projection = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        auto traversalLength = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        kernel->setOutput(projection, traversalLength);
        kernel->run();

        ImageSerializer::writeImage(testdataDirectory + "input_virtual_projection", projection, ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(testdataDirectory + "input_traversal_length", traversalLength, ImageSerializer::TIFF_GRAY_32);

        generateReferenceImage(testdataDirectory + "virtual_projection_reference", referenceConstantLength);

        delete traversalLength;
        delete projection;
        delete kernel;

        delete mappedVolume;
    }

    void generateReferenceImage(std::string fileName, float value)
    {
        std::vector<float> imageBuffer;
        for(unsigned int i=0; i < projectionResolution.x * projectionResolution.y; i++)
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

// 
// Long Object Compensation Tests
//
TEST_F(LongObjectCompensationTest, Unit_Long_Object_Compensation_Kernel_Constructor) 
{
    VirtualProjectionKernel* kernel = new VirtualProjectionKernel(framework, virtualProjection, traversalLength);
    delete kernel;
}

TEST_F(LongObjectCompensationTest, Unit_Long_Object_Compensation) 
{
    generateTestData();

    Image* virtualProjectionImage = ImageDeserializer::readImage(testdataDirectory + "input_virtual_projection.tif");
    Image* traversalLengthImage = ImageDeserializer::readImage(testdataDirectory + "input_traversal_length.tif");

    virtualProjection->setObjectOnCPU(virtualProjectionImage);
    traversalLength->setObjectOnCPU(traversalLengthImage);
    
    geometricSetup->setScannerGeometry(satRotator->createRotatedScannerGeometry(tiltAngleInDegrees));

    float constantLength = geometricSetup->getScannerGeometry()->getTraversalLengthThroughBoundingBox(volume->getProperties().getRealSpaceBoundingBox());
    ASSERT_LT(fabs(constantLength - referenceConstantLength), 0.01f);

    VirtualProjectionKernel* kernel = new VirtualProjectionKernel(framework, virtualProjection, traversalLength, constantLength);
    kernel->run();

    ImageSerializer::writeImage(workDirectory + "virtual_projection_generated", kernel->getProjection(), ImageSerializer::TIFF_GRAY_32);
    ImageSerializer::writeImage(workDirectory + "traversal_length_generated", traversalLength, ImageSerializer::TIFF_GRAY_32);

    Image* generatedVirutalProjection = ImageDeserializer::readImage(workDirectory + "virtual_projection_generated.tif");
    Image* referenceVirtualProjection = ImageDeserializer::readImage(testdataDirectory + "virtual_projection_reference.tif");

    ASSERT_TRUE(ImageComparator::areHeadersEqual(generatedVirutalProjection, referenceVirtualProjection));

    float error = ImageComparator::getRMS(generatedVirutalProjection, referenceVirtualProjection);

    ASSERT_LT(error, 0.1f);

    delete referenceVirtualProjection;
    delete generatedVirutalProjection;
    delete kernel;
    delete traversalLengthImage;
    delete virtualProjectionImage;
}
