#include "stdafx.h"
#include "io/serializer/VolumeSerializer.h"
#include "model/volume/VoxelType.h"
#include "io/datasource/MRCHeader.h"
#include "io/rangetransformation/LinearRangeTransformation.h"
#include "algorithm/volumestatistics/VolumeStatistics.h"
#include "framework/test/TestBase.h"

namespace ettention
{
    class VolumeSerializerTest : public TestBase
    {
    public:
        VolumeSerializerTest()
        {
            headerBytes.resize(sizeof(MRCHeader));
        }

        virtual ~VolumeSerializerTest()
        {
        }

        static MRCHeader* buildIMODReferenceHeader(Volume* volume)
        {
            MRCHeader* imodReferenceHeader = new MRCHeader();
            memset(imodReferenceHeader, '\0', sizeof(MRCHeader));
            imodReferenceHeader->nx = volume->Properties().GetVolumeResolution().x;
            imodReferenceHeader->ny = volume->Properties().GetVolumeResolution().z;
            imodReferenceHeader->nz = volume->Properties().GetVolumeResolution().y;

            imodReferenceHeader->mx = volume->Properties().GetVolumeResolution().x;
            imodReferenceHeader->my = volume->Properties().GetVolumeResolution().z;
            imodReferenceHeader->mz = volume->Properties().GetVolumeResolution().y;

            imodReferenceHeader->cellDimX = static_cast<float>(volume->Properties().GetVolumeResolution().x);
            imodReferenceHeader->cellDimY = static_cast<float>(volume->Properties().GetVolumeResolution().z);
            imodReferenceHeader->cellDimZ = static_cast<float>(volume->Properties().GetVolumeResolution().y);

            imodReferenceHeader->mode = 1;
            imodReferenceHeader->dMean = 222.f;
            imodReferenceHeader->dMax = 32767.f;
            imodReferenceHeader->dMin = -32768.f;

            imodReferenceHeader->map[0] = 'M';
            imodReferenceHeader->map[1] = 'A';
            imodReferenceHeader->map[2] = 'P';
            imodReferenceHeader->map[3] = ' ';

            imodReferenceHeader->mapC = 1;
            imodReferenceHeader->mapR = 2;
            imodReferenceHeader->mapS = 3;

            return imodReferenceHeader;
        }

        static bool areMRCHeadersEqual(const MRCHeader& headerA, const MRCHeader& headerB)
        {
            const char* bytesHeaderA = reinterpret_cast<const char*>(&headerA);
            const char* bytesHeaderB = reinterpret_cast<const char*>(&headerB);
            bool areEqual = true;
            for(unsigned int i = 0; i < sizeof(MRCHeader); i++)
            {
                areEqual = areEqual && (bytesHeaderA[i] == bytesHeaderB[i]);
            }

            return areEqual;
        }

        MRCHeader* extractHeaderFromMRCFile(const std::string filename)
        {
            MRCHeader* fileHeader = reinterpret_cast<MRCHeader*>(&(headerBytes[0]));
            char* headerBytesAsCharArray = reinterpret_cast<char*>(&(headerBytes[0]));

            std::ifstream mrcFile(filename.c_str(), std::ifstream::binary);
            if(!mrcFile.good())
            {
                std::string errorMessage = (boost::format("Unable to open file stored by VolumeSerializerTest %1%") % filename).str();
                throw std::runtime_error(errorMessage.c_str());
            }

            mrcFile.read(headerBytesAsCharArray, sizeof(MRCHeader));

            mrcFile.close();

            return fileHeader;
        }

        std::vector<char> headerBytes;
    };
}