#include "stdafx.h"
#include "Image.h"
#include <boost/numeric/conversion/bounds.hpp>
#include <cassert>
#include "gpu/opencl/CLMemBuffer.h"
#include "framework/error/Exception.h"

namespace ettention
{
    Image::Image(const Image* other)
        : resolution(other->resolution)
        , data(other->data)
    {
    }

    Image::Image(const Vec2ui& resolution, const float* initialData)
        : resolution(resolution)
    {
        if(resolution.x * resolution.y == 0)
            ETTENTION_THROW_EXCEPTION("Invalid image size: " << resolution.x << "x" << resolution.y << "!");

        data.resize(resolution.x * resolution.y);
        if(initialData)
        {
            data.assign(initialData, initialData + getPixelCount());
        }
    }
 
    Image::~Image()
    {
    }

    const Vec2ui& Image::getResolution() const
    {
        return resolution;
    }

    std::size_t Image::getPixelCount() const
    {
        return data.size();
    }

    std::size_t Image::getByteWidth() const
    {
        return getPixelCount() * sizeof(float);
    }

    float* Image::getData()
    {
        return &data[0];
    }

    const float* Image::getData() const
    {
        return &data[0];
    }

    float* Image::getScanLineData(unsigned int y)
    {
        return &data[y * resolution.x];
    }

    const float* Image::getScanLineData(unsigned int y) const
    {
        return &data[y * resolution.x];
    }

    void Image::convertToLogarithmicScale()
    {
        std::transform(data.begin(), data.end(), data.begin(), (float (*)(float))&std::log);
    }

    void Image::convertToLinearScale()
    {
        std::transform(data.begin(), data.end(), data.begin(), (float(*)(float))&std::exp);
    }

    float Image::getPixel(int x, int y) const
    {
        return data[y * resolution.x + x];
    }

    void Image::setPixel(size_t x, size_t y, float value)
    {
        data[y * resolution.x + x] = value;
    }

    float Image::computeAverageValue() const
    {
        return std::accumulate(data.begin(), data.end(), 0.0f) / (float)data.size();
    }

    float Image::findMinValue() const
    {
        return *std::min_element(data.begin(), data.end());
    }

    float Image::findMaxValue() const
    {
        return *std::max_element(data.begin(), data.end());
    }

    float Image::computeGaussFilteredPixel(const Vec2i& position, int radius) const
    {
        unsigned int n = 0;
        float sum = 0.0f;
        for(int x = position.x - radius; x <= position.x + radius; x++)
        {
            for(int y = position.y - radius; y <= position.y + radius; y++)
            {
                if(x < 0 || y < 0 || x >= (int)getResolution().x || y >= (int)getResolution().y)
                    continue;

                float distanceX = (float)(position.x - x);
                float distanceY = (float)(position.y - y);
                if(distanceX * distanceX + distanceY * distanceY > radius * radius)
                    continue;

                sum += getPixel(x, y);
                ++n;
            }
        }
        return sum / (float)n;
    }
}