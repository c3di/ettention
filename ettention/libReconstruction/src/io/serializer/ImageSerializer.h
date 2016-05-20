/****************************************************************************************
*
* ETtention Framework for Reconstruction From Projections
* (c) 2012 Copyright by German Research Center for Artificial Intelligence, DFKI GmbH
*
* License: see LICENSE.txt in main project directory
* Authors: Lukas Marsalek, Tim Dahmen, Beata Turonova
*
*****************************************************************************************/

#pragma once

#include "model/image/Image.h"
#include "gpu/GPUMapped.h"

namespace ettention
{
    class ImageSerializer
    {
    public:
        enum ImageFormat
        {
            TIFF_GRAY_32, BPM_GRAY_8, PNG_GRAY_8, PNG_24
        };

        //! deprecated todo remove
        static void serializeGrayscale(std::string filenameTrunk, float* aData, unsigned int aWidth, unsigned int aHeight, float buffMin, float buffMax);
        //! deprecated todo remove
        static void serializeGrayscale(std::string filenameTrunk, float* aData, unsigned int aWidth, unsigned int aHeight);

        static void writeImage(std::string filenameTrunk, const Image* image, ImageFormat format);
        static void writeImage(std::string filenameTrunk, GPUMapped<Image>* image, ImageFormat format);

    protected:
        ImageSerializer();
        virtual ~ImageSerializer();

    private:
        static std::string getFileName(std::string filenameTrunk, ImageFormat format);
        static std::pair<float, float> computeExtremalValuesOfFloatBuffer(float* data, unsigned int size);
        static void writePPM(const std::string& filename, const std::string& imageHeader, const std::vector<unsigned char>& imageData);
        static std::string constructGrayscalePPMHeader(size_t width, size_t height);
        static std::vector<unsigned char> scaleDataToBytes(float* data, size_t width, size_t height, float dataMinValue, float dataMaxValue);
        static std::string constructFilename(std::string& filenameTrunk);
        static bool isFormat8bit(ImageFormat format);
        static bool isFormatPNG(ImageFormat format);
    };
}