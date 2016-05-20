#include "stdafx.h"

#include "ImageDeserializer.h"

#include "framework/error/Exception.h"
#include "FreeImagePlus.h"

namespace ettention
{
    Image* ImageDeserializer::readImage(std::string filename)
    {
        fipImage fimg(FIT_FLOAT);
        if(!fimg.load(filename.c_str())) {
            std::string message("unable to load image " + filename);
            throw std::runtime_error(message.c_str());
        }

        return convertFipImage(fimg, filename);
    }

    unsigned int ImageDeserializer::getNumberOfImagesInStack(std::string filename)
    {
        fipMultiPage imageFile;
        imageFile.open(filename.c_str(), false, true);
        unsigned int numberOfImages = imageFile.getPageCount();
        return numberOfImages;
    }

    Image* ImageDeserializer::readImageFromStack(std::string filename, unsigned int index)
    {
        fipMultiPage imageFile(true);
        if(!imageFile.open(filename.c_str(), false, true))
            throw Exception("unable to open image stack " + filename);

        unsigned int numberOfImages = imageFile.getPageCount();
        if(index > numberOfImages)
            throw Exception("index out of bound: image stack does not have that many images");

        fipImage fimg;
        fimg = imageFile.lockPage(index);
        if(!fimg.isValid())
            throw Exception("failed to load image from stack");

        Image* image = convertFipImage(fimg, filename);

        imageFile.unlockPage(fimg, true);
        imageFile.close();

        return image;
    }

    Image* ImageDeserializer::convertFipImage(fipImage& fimg, std::string filename)
    {
        // need to create tmp fimg because of an bug in FreeImage
        fipImage tmp(fimg);
        tmp.convertToFloat();
        Vec2ui resolution(tmp.getWidth(), tmp.getHeight());
        Image* image = new Image(resolution);
        for(unsigned int y = 0; y < resolution.y; y++)
        {
            // freeImage reads the images flipped vertically, so flip them back
            float* tgtPtr = image->getScanLineData(resolution.y - y - 1);
            memcpy(tgtPtr, tmp.getScanLine(y), resolution.x * sizeof(float));
        }
        return image;
    }

    ImageDeserializer::ImageDeserializer()
    {
    }


    ImageDeserializer::~ImageDeserializer()
    {
    }
}