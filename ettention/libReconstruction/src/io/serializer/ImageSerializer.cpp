#include "stdafx.h"
#include <FreeImagePlus.h>
#include <boost/format.hpp>
#include "io/serializer/ImageSerializer.h"
#include "framework/Logger.h"
#include "framework/error/Exception.h"

namespace ettention
{
    void ImageSerializer::serializeGrayscale(std::string filenameTrunk, float* aData, unsigned int aWidth, unsigned int aHeight, float dataMin, float dataMax)
    {
        try
        {
            std::string filename = constructFilename(filenameTrunk);
            std::vector<unsigned char> imageData = scaleDataToBytes(aData, aWidth, aHeight, dataMin, dataMax);
            std::string imageHeader = constructGrayscalePPMHeader(aWidth, aHeight);

            writePPM(filename, imageHeader, imageData);

        }
        catch(const std::exception& e)
        {
            LOGGER_ERROR(e.what());
        }
        catch(...)
        {
            LOGGER_ERROR("Unknown PPM serialization error");
        }
    }

    void ImageSerializer::serializeGrayscale(std::string filenameTrunk, float* aData, unsigned int aWidth, unsigned int aHeight)
    {
        std::pair<float, float> extrema = computeExtremalValuesOfFloatBuffer(aData, aWidth * aHeight);
        ImageSerializer::serializeGrayscale(filenameTrunk, aData, aWidth, aHeight, extrema.first, extrema.second);
    }

    void ImageSerializer::writeImage(std::string filenameTrunk, const Image* image, ImageSerializer::ImageFormat format)
    {
        fipImage fimg(FIT_FLOAT, image->getResolution().x, image->getResolution().y, 32);

        for(unsigned int y = 0; y < fimg.getHeight(); y++)
        {
            memcpy(fimg.getScanLine(y), image->getScanLineData(y), fimg.getWidth() * sizeof(float));
        }

        std::string filename = getFileName(filenameTrunk, format);
        if(!fimg.isValid())
        {
            throw std::runtime_error(("Image to save to " + filename + " is invalid!").c_str());
        }

        if(isFormat8bit(format))
        {
            fimg.convertToType(FIT_BITMAP);
        }

        fimg.flipVertical();
        if(!fimg.save(filename.c_str()))
        {
            throw Exception((boost::format("Saving of image to %1% failed!") % filename).str());
        }
    }

    void ImageSerializer::writeImage(std::string filenameTrunk, GPUMapped<Image>* image, ImageSerializer::ImageFormat format)
    {
        image->ensureIsUpToDateOnCPU();
        writeImage(filenameTrunk, image->getObjectOnCPU(), format);
    }

    std::string ImageSerializer::getFileName(std::string filenameTrunk, ImageSerializer::ImageFormat format)
    {
        if(format == TIFF_GRAY_32)
            return filenameTrunk + ".tif";
        if(format == BPM_GRAY_8)
            return filenameTrunk + ".bpm";
        if(format == PNG_GRAY_8)
            return filenameTrunk + ".png";
        if(format == PNG_24)
            return filenameTrunk + ".png";
        throw std::runtime_error("image format not supported");
    }

    std::string ImageSerializer::constructFilename(std::string& filenameTrunk)
    {
        return (boost::format("%1%.pgm") % filenameTrunk).str();
    }

    std::vector<unsigned char> ImageSerializer::scaleDataToBytes(float* data, size_t width, size_t height, float dataMinValue, float dataMaxValue)
    {
        size_t numPixels = width * height;
        std::vector<unsigned char> byteBuffer;
        byteBuffer.reserve(numPixels);

        float dataRangeInverse = 1.0f / (dataMaxValue - dataMinValue);
        for(unsigned int i = 0; i < numPixels; i++)
        {
            byteBuffer.push_back(static_cast<unsigned char>(((data[i] - dataMinValue) * dataRangeInverse) * 255.f));
        }

        return byteBuffer;
    }

    std::string ImageSerializer::constructGrayscalePPMHeader(size_t width, size_t height)
    {
        return (boost::format("P5 %1% %2% 255 ") % width % height).str();
    }

    std::pair<float, float> ImageSerializer::computeExtremalValuesOfFloatBuffer(float* data, unsigned int size)
    {
        std::pair<float, float> extrema(std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest());
        for(unsigned int i = 0; i < size; i++) {
            if(data[i] < extrema.first)
                extrema.first = data[i];
            if(data[i] > extrema.second)
                extrema.second = data[i];
        }
        return extrema;
    }

    void ImageSerializer::writePPM(const std::string& filename, const std::string& imageHeader, const std::vector<unsigned char>& imageData)
    {
        std::ofstream ppmFileStream(filename.c_str(), std::ofstream::binary);
        if(!ppmFileStream.good())
        {
            std::string errorMessage = (boost::format("Unable to open file for PPM serialization %1%") % filename).str();
            throw std::runtime_error(errorMessage.c_str());
        }

        std::ostreambuf_iterator<char> ppmFileIterator(ppmFileStream);

        std::copy(imageHeader.begin(), imageHeader.end(), ppmFileIterator);
        std::copy(imageData.begin(), imageData.end(), ppmFileIterator);

        ppmFileStream.close();
    }

    ImageSerializer::ImageSerializer()
    {
    }


    ImageSerializer::~ImageSerializer()
    {
    }

    bool ImageSerializer::isFormat8bit(ImageFormat format)
    {
        if((format == BPM_GRAY_8) || (format == PNG_GRAY_8))
        {
            return true;
        }

        return false;
    }

    bool ImageSerializer::isFormatPNG(ImageFormat format)
    {
        return (format == PNG_GRAY_8 || format == PNG_24);
    }
}