#pragma once

#include <cstdlib>
#include <string>
#include "framework/evaluation/RootMeanSquareError.h"
#include "model/image/Image.h"

namespace ettention
{
    class ImageComparator
    {
    public:
        static bool areHeadersEqual(Image* first, Image* second);

        static float getRMS(std::string firstFilename, std::string secondFilename);
        static float getRMS(Image* first, Image* second);

        static float getMaxError(std::string firstFilename, std::string secondFilename);
        static float getMaxError(Image* first, Image* second);

        static void assertImagesAreEqual(std::string firstFilename, std::string secondFilename);
        static void assertImagesAreEqual(Image* first, Image* second);

    protected:
        ImageComparator();
        ~ImageComparator();
    };
}