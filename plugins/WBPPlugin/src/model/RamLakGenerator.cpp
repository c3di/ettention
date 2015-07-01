#include "stdafx.h"
#include "RamLakGenerator.h"
#include <framework/math/Vec2.h>
#include <gpu/GPUMapped.h>
#include <model/image/Image.h>

namespace ettention
{
    namespace wbp
    {
        GPUMapped<Image>* RamLakGenerator::generateKernel(CLAbstractionLayer* clStack, const Vec2ui& kernelResolution)
        {
            int halfKernelWidth = (int)(kernelResolution.x / 2);
            int halfKernelHeight = (int)(kernelResolution.y / 2);

            std::vector<float> kernelValues;

            for(int x = -halfKernelWidth; x <= halfKernelWidth; x++)
            {
                for(int y = -halfKernelHeight; y <= halfKernelHeight; y++)
                {
                    float value;
                    int position = std::abs(x) + std::abs(y);

                    if(x == 0 && y == 0)
                        value = 1.0f / 4.0f;
                    else if(position % 2 == 0)
                        value = 0.0f;
                    else
                        value = -1.0f / ((float)(position * position) * (float)(M_PI * M_PI));

                    kernelValues.push_back(value);
                }
            }
            auto kernelOnCPU = new Image(Vec2ui(2 * halfKernelWidth + 1, 2 * halfKernelHeight + 1), &kernelValues[0]);
            auto kernel = new GPUMapped<Image>(clStack, kernelOnCPU);
            kernel->takeOwnershipOfObjectOnCPU();
            return kernel;
        }
    }
}