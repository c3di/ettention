#include "stdafx.h"

#include "model/image/Image.h"
#include "algorithm/imagestatistics/histogram/HistogramEqualization.h"

namespace ettention
{
    Image* HistogramEqualization::equalizeHistograms(Image* imageForHistogram, Image* imageToBeAdapted)
    {
        std::vector<unsigned int> indexHistogramImage = sortIndices<float>(imageForHistogram->getData(), imageForHistogram->getResolution().x * imageForHistogram->getResolution().y);
        std::vector<unsigned int> indexImageToBeAdapted = sortIndices<float>(imageToBeAdapted->getData(), imageForHistogram->getResolution().x * imageForHistogram->getResolution().y);

        std::vector<float> equalizedData(imageForHistogram->getResolution().x * imageForHistogram->getResolution().y);

        for(unsigned int i = 0; i < indexHistogramImage.size(); i++)
        {
            equalizedData[indexImageToBeAdapted[i]] = *(imageForHistogram->getData() + indexHistogramImage[i]);
        }

        return new Image(imageForHistogram->getResolution(), &equalizedData[0]);
    }
}