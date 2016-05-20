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

    void MRCWriter::writeHeader(std::ofstream &outfile, VoxelValueType fileMode, VolumeProperties& volumeProperties, const Vec3ui volumeResolution, RangeTransformation* rangeTransformation)
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
