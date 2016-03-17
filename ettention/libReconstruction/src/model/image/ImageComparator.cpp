#include "stdafx.h"

#include "ImageComparator.h"

#include "framework/evaluation/RootMeanSquareError.h"
#include "io/deserializer/ImageDeserializer.h"
#include "framework/error/Exception.h"

#include <boost/math/special_functions.hpp>

namespace ettention
{
    ImageComparator::ImageComparator()
    {
    }

    ImageComparator::~ImageComparator()
    {
    }

    bool ImageComparator::areHeadersEqual(Image* first, Image* second)
    {
        return first->getResolution() == second->getResolution();
    }

    float ImageComparator::getRMS(std::string firstFilename, std::string secondFilename)
    {
        Image* firstImage = ImageDeserializer::readImage(firstFilename);
        Image* secondImage = ImageDeserializer::readImage(secondFilename);
        float rms = getRMS(firstImage, secondImage);
        delete secondImage;
        delete firstImage;
        return rms;
    }

    float ImageComparator::getRMS(Image* first, Image* second)
    {
        if(first->getResolution() != second->getResolution())
            throw Exception("Image dimensions do not match!");

        return RootMeanSquareError::computeRMSError(first->getData(), second->getData(), first->getPixelCount());
    }

    float ImageComparator::getMaxError(std::string firstFilename, std::string secondFilename)
    {
        Image* first = ImageDeserializer::readImage(firstFilename);
        Image* second = ImageDeserializer::readImage(secondFilename);

        float result = ImageComparator::getMaxError(first, second);

        delete second;
        delete first;

        return result;
    }

    float ImageComparator::getMaxError(Image* first, Image* second)
    {
        float result = 0.0f;
        float rmsError = 0.0f;

        if(first->getResolution() != second->getResolution())
            throw std::runtime_error("Illegal comparison of images with different resolution!");

        float minValue = FLT_MAX;
        float maxValue = -FLT_MAX;

        for(unsigned int i = 0; i < first->getPixelCount(); i++)
        {
            if(!boost::math::isfinite(first->getData()[i]))
                throw std::runtime_error("illegal value in data");
            if(!boost::math::isfinite(second->getData()[i]))
                throw std::runtime_error("illegal value in data");

            if(second->getData()[i] < minValue)
                minValue = second->getData()[i];
            if(second->getData()[i] > maxValue)
                maxValue = second->getData()[i];

            rmsError = (first->getData()[i] - second->getData()[i]) * (first->getData()[i] - second->getData()[i]);

            if(rmsError > result)
            {
                result = rmsError;
            }
        }

        return result;
    }

    void ImageComparator::assertImagesAreEqual(std::string firstFilename, std::string secondFilename)
    {
        Image* first = ImageDeserializer::readImage(firstFilename);
        Image* second = ImageDeserializer::readImage(secondFilename);

        try
        {
            ImageComparator::assertImagesAreEqual(first, second);
        }
        catch(const std::exception &e )
        {
            throw Exception("Assertion between " + firstFilename + " and " + secondFilename + " failed. " + e.what());
        }

        delete second;
        delete first;
    }

    void ImageComparator::assertImagesAreEqual(Image *firstImage, Image *secondImage)
    {
        const float rms = getRMS(firstImage, secondImage);
        if(rms > 0.1f)
            ETTENTION_THROW_EXCEPTION("Images are different, according to high cumulative RMS error (" << rms << ")!");

        const float maxPixelIntensity = std::fmaxf(firstImage->findMaxValue(), secondImage->findMaxValue());
        const float threshold = maxPixelIntensity * 0.00001f;
        const float maxError = getMaxError(firstImage, secondImage);
        if(maxError > threshold)
        {
            throw Exception("Images are different, according to large difference between two corresponding values. ");
        }
    }
}