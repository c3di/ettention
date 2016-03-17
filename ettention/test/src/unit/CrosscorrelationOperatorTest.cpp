#include "stdafx.h"

#include "framework/test/ImageManipulationTestBase.h"
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationOperator.h"
#include "algorithm/imagemanipulation/ThresholdKernel.h"

#include "algorithm/imagestatistics/ThresholdDecider.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSimpleKernel.h"

#include "algorithm/imagestatistics/particledetection/ParticleExtractor.h"

#include "io/deserializer/ImageDeserializer.h"
#include "io/serializer/ImageSerializer.h"

#include "model/image/Image.h"
#include "model/image/ImageComparator.h"

#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/GPUMapped.h"

using namespace ettention;

class CrosscorrelationOperatorTest : public ImageManipulationTestBase
{
public:
    CrosscorrelationOperatorTest()
        : ImageManipulationTestBase(std::string("NCC"), std::string(TESTDATA_DIR"/data/alignment"), std::string(TESTDATA_DIR"/work/alignment/ncc"))
    {
    }

    virtual void logOperatorProperties(CrosscorrelationOperator *crossOp)
    {
        Vec2f refInfo = crossOp->getReferenceImageInfo();
        Vec2f cndInfo = crossOp->getCandidateImageInfo();
        Vec2ui pos = crossOp->getPeakPosition();
        float peak = crossOp->getPeakValue();

        std::stringstream sstr;
        sstr << "Peak:" << peak << " at[" << pos.x << ", " << pos.y << "]; Reference[" << refInfo << "]. Candidate[" << cndInfo << "]." << std::endl;
        LOGGER(sstr.str().c_str());
    }

    virtual void debugImageOut(std::string groupName, GPUMapped<Image> *reference, GPUMapped<Image> *candidate, GPUMapped<Image> *result)
    {
        this->saveImageAsFile(groupName + "_0src", reference);
        this->saveImageAsFile(groupName + "_1cnd", candidate);
        this->saveImageAsFile(groupName + "_2res", result);
    }

    virtual void nccOperatorTemplateMatchingCase(std::string caseName, std::string referenceFileName, std::string candidateFileName)
    {
        auto *srcImage = this->loadInputImageFileToGPU(referenceFileName);
        auto *cndImage = this->loadInputImageFileToGPU(candidateFileName);

        CrosscorrelationOperator *crossOp = new CrosscorrelationOperator(framework, srcImage, cndImage);
        crossOp->matchTemplate();

        GPUMapped<Image> *result = crossOp->getOutput();

        LOGGER("Template Matching NCC: " + caseName);
        result->ensureIsUpToDateOnCPU();
        this->debugImageOut(caseName, srcImage, cndImage, result);
        this->logOperatorProperties(crossOp);

        delete crossOp;

        delete srcImage;
        delete cndImage;
    }

    virtual Vec2ui nccOperatorTranslationCase(std::string caseName, std::string referenceFileName, std::string candidateFileName)
    {
        Vec2ui result;
        auto *srcImage = this->loadInputImageFileToGPU(referenceFileName);
        auto *cndImage = this->loadInputImageFileToGPU(candidateFileName);

        CrosscorrelationOperator *crossOp = new CrosscorrelationOperator(framework, srcImage, cndImage);
        crossOp->findTranslation();

        GPUMapped<Image> *resultImage = crossOp->getOutput();

        LOGGER("Translation NCC: " + caseName);
        resultImage->ensureIsUpToDateOnCPU();
        this->debugImageOut(caseName, srcImage, cndImage, resultImage);
        this->logOperatorProperties(crossOp);

        result = crossOp->getPeakPosition();
        delete crossOp;

        delete cndImage;
        delete srcImage;

        return result;
    }

};

//
// CrossCorrelation Tests
//
TEST_F(CrosscorrelationOperatorTest, NCC_Constructor)
{
    Vec2ui projectionSize(256, 256);
    Vec2ui patchSize(33, 33);

    auto emptySource = new GPUMapped<Image>(this->framework->getOpenCLStack(), projectionSize);

    CrosscorrelationOperator* crossOp = new CrosscorrelationOperator(this->framework, emptySource, emptySource);
    delete crossOp;

    delete emptySource;
}

TEST_F(CrosscorrelationOperatorTest, NCC_Case)
{
    this->nccOperatorTemplateMatchingCase("debug", "marker_pattern_white_11", "marker_pattern_black_11");
}

TEST_F(CrosscorrelationOperatorTest, NCC_Correlation)
{
    this->nccOperatorTemplateMatchingCase("correlation", "HPFCere2a_z1", "marker_pattern_white_11");
}

TEST_F(CrosscorrelationOperatorTest, NCC_Autocorrelation)
{
    auto answer = this->nccOperatorTranslationCase("autocorrelation", "projection_convergent", "projection_convergent");
    auto truth = Vec2ui(0, 0);
    ASSERT_TRUE(answer == truth);
}

TEST_F(CrosscorrelationOperatorTest, NCC_Translate)
{
    this->nccOperatorTranslationCase("translation", "synt_166value_128p", "synt_shift16right_166value_128p");
}
