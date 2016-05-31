#include "stdafx.h"
#include "PreFilterOperator.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionFourierSpaceImplementation.h"
#include "framework/Framework.h"
#include "framework/math/Matrix4x4.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "reconstruction/projections/AdjointBackProjectionKernel.h"
#include "reconstruction/GenerateCircularPSFKernel.h"

namespace ettention
{
    namespace stem
    {
        PrefilterOperator::PrefilterOperator(Framework* framework, GPUMapped<Image>* residual, float maxRadius)
            : clContext(framework->getOpenCLStack())
            , residual(residual)
            , maxRadius(maxRadius)
        {
            if( residual->getResolution().y > 8192 )
                throw Exception((boost::format("Resolution (%1%) from PrefilterOperator::PrefilterOperator is too huge to be real") % residual->getResolution()).str());

            circleGenerator = new GenerateCircularPSFKernel(framework, residual->getResolution(), 0.0f);
            convolutionOperator = new ConvolutionFourierSpaceImplementation(framework, residual, circleGenerator->getOutput(), false);

            prefilteredResiduals = new GPUMapped<Image>(clContext, new Image(getTotalResolutionOfPrefilteredResidual()));
            prefilteredResiduals->takeOwnershipOfObjectOnCPU();
        }

        PrefilterOperator::~PrefilterOperator()
        {
            delete prefilteredResiduals;
            delete convolutionOperator;
            delete circleGenerator;
        }

        float PrefilterOperator::getRadiusOfSamplingStep(int i)
        {
            return powf(2.0f, ((float)i) - 1.0f);
        }

        unsigned int PrefilterOperator::NumberOfSamplingSteps()
        {
            unsigned int log2MaxRadius = (unsigned int)(logf(maxRadius) / logf(2.0f));
            return log2MaxRadius + 2;
        }

        void PrefilterOperator::run()
        {
            residual->ensureIsUpToDateOnGPU();
            attachUnfilteredResidual();
            auto numberOfSteps = NumberOfSamplingSteps();
            for( unsigned int i = 1; i < numberOfSteps; i++ )
            {
                computeAdjointOperator(i);
                convolutionOperator->execute();
                attachAdjointOperatorAtPosition(i);
            }
            prefilteredResiduals->markAsChangedOnGPU();
        }

        void PrefilterOperator::computeAdjointOperator(int i)
        {
            circleGenerator->setRadius(getRadiusOfSamplingStep(i));
            circleGenerator->run();
        }

        void PrefilterOperator::attachUnfilteredResidual()
        {
            residual->getBufferOnGPU()->copyTo(prefilteredResiduals->getBufferOnGPU(), 0, 0, residual->getBufferOnGPU()->getByteWidth());
        }

        void PrefilterOperator::attachAdjointOperatorAtPosition(int i)
        {
            auto bufferSize = convolutionOperator->getOutput()->getObjectOnCPU()->getByteWidth();
            convolutionOperator->getOutput()->ensureIsUpToDateOnGPU();
            convolutionOperator->getOutput()->getBufferOnGPU()->copyTo(prefilteredResiduals->getBufferOnGPU(), 0, i * bufferSize, bufferSize);
        }

        GPUMapped<Image>* PrefilterOperator::getOutput()
        {
            return prefilteredResiduals;
        }

        float PrefilterOperator::getMaxRadius() const
        {
            return maxRadius;
        }

        void PrefilterOperator::setMaxRadius(float val)
        {
            maxRadius = val;
            updateOutputBufferResolution();
        }

        Vec2ui PrefilterOperator::getTotalResolutionOfPrefilteredResidual()
        {
            Vec2ui totalResolutionOfPrefilteredResidual = residual->getResolution();
            totalResolutionOfPrefilteredResidual.y *= NumberOfSamplingSteps();
            return totalResolutionOfPrefilteredResidual;
        }

        void PrefilterOperator::updateOutputBufferResolution()
        {
            auto updatedResolution = getTotalResolutionOfPrefilteredResidual();
            if( updatedResolution.y > 16386 )
                throw Exception( (boost::format("TRAP IN PrefilterOperator::updateOutputBufferResolution CATCHED suspicious resolution (%1% too huge to be real) FROM  getTotalResolutionOfPrefilteredResidual()") % updatedResolution).str());

            try
            {
                prefilteredResiduals->setObjectOnCPU(new Image(updatedResolution));
            } catch( const std::bad_alloc& e )
            {
                std::cout << (boost::format("TRAP IN PrefilterOperator::updateOutputBufferResolution CATCHED bad_alloc DURING new Image(%1%)") % updatedResolution).str() << std::endl;
                throw e;
            }
            prefilteredResiduals->takeOwnershipOfObjectOnCPU();
        }

    }
}