#include "stdafx.h"
#include "framework/error/Exception.h"
#include "io/serializer/MRCWriter.h"
#include "io/datasource/MRCHeader.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace ettention
{
    MRCWriter::MRCWriter()
    {
    }

    MRCWriter::~MRCWriter()
    {
    }

    bool MRCWriter::canGenerateVoxelType(VoxelValueType mode)
    {
        switch( mode )
        {
        case ettention::IO_VOXEL_TYPE_UNSIGNED_8:
        case ettention::IO_VOXEL_TYPE_UNSIGNED_16:
        case ettention::IO_VOXEL_TYPE_FLOAT_32:
            return true;

        case ettention::IO_VOXEL_TYPE_UNSPECIFIED:
        case ettention::IO_VOXEL_TYPE_SIGNED_16:
        default:
            return false;
        }

        return false;
    }

    bool MRCWriter::canGenerateRotation(CoordinateOrder rotation)
    {
        return true;
    }

    std::string MRCWriter::getFileExtension()
    {
        return ".mrc";
    }

    void MRCWriter::write(Volume* volume, boost::filesystem::path outputVolumeFileName, const OutputParameterSet* options)
    {
        RangeTransformation* rangeTransformation = createRangeTransformation(volume, options);
        auto volumeDataInFloatFormat = volume->convertToFloat();
        rangeTransformation->transformRange(volumeDataInFloatFormat.get(), volume->getProperties().getVolumeVoxelCount());

        unsigned int orderDependentIndices[3];
        fillOrderDependentIndices(orderDependentIndices, options->getOrientation());
        Vec3ui outputVolumeResolution;
        outputVolumeResolution.z = volume->getProperties().getVolumeResolution()[orderDependentIndices[0]];
        outputVolumeResolution.y = volume->getProperties().getVolumeResolution()[orderDependentIndices[1]];
        outputVolumeResolution.x = volume->getProperties().getVolumeResolution()[orderDependentIndices[2]];

        std::ofstream outfile;
        outfile.open(outputVolumeFileName.string(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if(!outfile.good())
        {
            throw Exception("Failed to open MRC file " + outputVolumeFileName.string() + " for writing!");
        }
        writeMRCHeader(outfile, options->getVoxelType(), volume->getProperties(), outputVolumeResolution, rangeTransformation);
        delete rangeTransformation;

        for(unsigned int i = 0; i < outputVolumeResolution.z; ++i)
        {
            for(unsigned int j = 0; j < outputVolumeResolution.y; ++j)
            {
                for(unsigned int k = 0; k < outputVolumeResolution.x; ++k)
                {
                    Vec3ui coords;
                    coords[orderDependentIndices[0]] = i;
                    coords[orderDependentIndices[1]] = j;
                    coords[orderDependentIndices[2]] = k;
                    writeValue(outfile, volumeDataInFloatFormat.get()[volume->getVoxelIndex(coords)], options->getVoxelType());
                }
            }
        }

        if(!outfile.good())
        {
            throw Exception((boost::format("error writing to output file %1%") % outputVolumeFileName).str());
        }
        outfile.close();
    }

    void MRCWriter::writeMRCHeader(std::ofstream &outfile, VoxelValueType fileMode, VolumeProperties& volumeProperties, const Vec3ui volumeResolution, RangeTransformation* rangeTransformation)
    {
        MRCHeader header;
        memset(&header, 0, sizeof(MRCHeader));

        header.mode = fileMode;

        header.nx = volumeResolution.x;
        header.ny = volumeResolution.y;
        header.nz = volumeResolution.z;

        header.mx = volumeResolution.x;
        header.my = volumeResolution.y;
        header.mz = volumeResolution.z;

        header.cellDimX = (float)volumeResolution.x;
        header.cellDimY = (float)volumeResolution.y;
        header.cellDimZ = (float)volumeResolution.z;

        header.map[0] = 'M';
        header.map[1] = 'A';
        header.map[2] = 'P';
        header.map[3] = ' ';

        header.mapC = 1;
        header.mapR = 2;
        header.mapS = 3;
        header.extra = 0;
        header.nint = 0;
        header.nreal = 0;
        header.dMin = rangeTransformation->getTransformedStatistics().getMin();
        header.dMax = rangeTransformation->getTransformedStatistics().getMax();
        header.dMean = rangeTransformation->getTransformedStatistics().getMean();

        outfile.write((const char*)&header, sizeof(MRCHeader));
    }
}
