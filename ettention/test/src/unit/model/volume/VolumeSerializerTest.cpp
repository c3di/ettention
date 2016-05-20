#include "stdafx.h"
#include "io/serializer/VolumeSerializer.h"
#include "model/volume/Voxel.h"
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
            imodReferenceHeader->nx = volume->getProperties().getVolumeResolution().x;
            imodReferenceHeader->ny = volume->getProperties().getVolumeResolution().z;
            imodReferenceHeader->nz = volume->getProperties().getVolumeResolution().y;

            imodReferenceHeader->mx = volume->getProperties().getVolumeResolution().x;
            imodReferenceHeader->my = volume->getProperties().getVolumeResolution().z;
            imodReferenceHeader->mz = volume->getProperties().getVolumeResolution().y;

            imodReferenceHeader->cellDimX = static_cast<float>(volume->getProperties().getVolumeResolution().x);
            imodReferenceHeader->cellDimY = static_cast<float>(volume->getProperties().getVolumeResolution().z);
            imodReferenceHeader->cellDimZ = static_cast<float>(volume->getProperties().getVolumeResolution().y);

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