#include "stdafx.h"
#include <framework/test/TestBase.h>
#include <framework/NumericalAlgorithms.h>
#include <framework/error/GeometricComputationException.h>
#include <framework/math/Float16Compressor.h>
#include <framework/test/HalfFloatTestKernel.h>
#include <cmath>

#define EPS 1e-5f

using namespace ettention;

class NumericalAlgorithmsTest : public TestBase
{
public:
    NumericalAlgorithmsTest() {}
    ~NumericalAlgorithmsTest() {}
};

TEST_F(NumericalAlgorithmsTest, RayVoxelIntersectionLength)
{
    Vec3f unitVoxel(1.0f, 1.0f, 1.0f);
    Vec3f diagonalDir(1.0f, 1.0f, -1.0f);
    Vec3f planeDiagonalDir(0.0f, -1.0f, 1.0f);
    Vec3f axisDir(0.0f, 0.0f, 1.0f);

    float diagonalUnitVoxelSize = NumericalAlgorithms::getIntersectionLengthOfRayThroughVoxelCenter(diagonalDir, unitVoxel);
    ASSERT_NEAR(diagonalUnitVoxelSize, std::sqrt(3.0f), EPS);

    float diagonalScaledVoxelSize = NumericalAlgorithms::getIntersectionLengthOfRayThroughVoxelCenter(diagonalDir, 2.0f * unitVoxel);
    ASSERT_NEAR(diagonalScaledVoxelSize, 2.0f * std::sqrt(3.0f), EPS);
    
    float axisUnitVoxelSize = NumericalAlgorithms::getIntersectionLengthOfRayThroughVoxelCenter(axisDir, unitVoxel);
    ASSERT_NEAR(axisUnitVoxelSize, 1.0f, EPS);

    float planeDiagonalUnitVoxelSize = NumericalAlgorithms::getIntersectionLengthOfRayThroughVoxelCenter(planeDiagonalDir, unitVoxel);
    ASSERT_NEAR(planeDiagonalUnitVoxelSize, std::sqrt(2.0f), EPS);

    ASSERT_THROW(NumericalAlgorithms::getIntersectionLengthOfRayThroughVoxelCenter(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 1.0f)), GeometricComputationException);
}

TEST_F(NumericalAlgorithmsTest, HalfFloat_Compress_Decompress_Identity)
{
    std::vector<float> inputData = { 0.0f, 1.0f, 2.0f, -1.0f, -2.0f, std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() };
    std::vector<unsigned short> halfFloatRepresentation(inputData.size());
    std::vector<float> checkData(inputData.size());

    Float16Compressor::compress(&inputData[0], &halfFloatRepresentation[0], inputData.size());
    Float16Compressor::decompress(&halfFloatRepresentation[0], &checkData[0], inputData.size());

    for(unsigned int i = 0; i < inputData.size(); i++)
        ASSERT_FLOAT_EQ(inputData[i], checkData[i]);
}

TEST_F(NumericalAlgorithmsTest, HalfFloat_Arithmetic_on_GPU)
{
    std::vector<float> inputData = { 0.0f, 1.0f, 2.0f, -1.0f, -2.0f, std::numeric_limits<float>::infinity() };
    std::vector<float> checkData(inputData.size());
    VectorAdaptor<unsigned short> halfFloatRepresentation((unsigned int) inputData.size());
    Float16Compressor::compress(&inputData[0], &halfFloatRepresentation[0], inputData.size());

    GPUMappedUShortVector gpuBuffer(framework->getOpenCLStack(), &halfFloatRepresentation);

    HalfFloatTestKernel kernel(framework, &gpuBuffer);
    kernel.run();

    gpuBuffer.ensureIsUpToDateOnCPU();
    Float16Compressor::decompress(&halfFloatRepresentation[0], &checkData[0], inputData.size());

    ASSERT_FLOAT_EQ(checkData[0], 1.0f);
    ASSERT_FLOAT_EQ(checkData[1], 0.5f);
    ASSERT_FLOAT_EQ(checkData[2], 4.0f);
    ASSERT_FLOAT_EQ(checkData[3], 1.0f);
    ASSERT_FLOAT_EQ(checkData[4], 4.0f);
    ASSERT_FLOAT_EQ(checkData[5], std::numeric_limits<float>::infinity());
    
}
