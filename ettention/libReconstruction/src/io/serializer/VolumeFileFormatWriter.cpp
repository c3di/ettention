#include "stdafx.h"
#include "framework/error/Exception.h"
#include "io/rangetransformation/IdentityRangeTransformation.h"
#include "io/rangetransformation/LinearRangeTransformation.h"
#include "io/serializer/VolumeFileFormatWriter.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace ettention
{

    void VolumeFileFormatWriter::write(Volume* volume, boost::filesystem::path outputVolumeFileName, const OutputParameterSet* options)
    {
        std::ofstream outfile;
        outfile.open(outputVolumeFileName.string(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if( !outfile.good() )
        {
            throw Exception((boost::format("Failed to open output file %1% for writing!") % outputVolumeFileName).str());
        }

        RangeTransformation* rangeTransformation = createRangeTransformation(volume, options);
        auto volumeDataInFloatFormat = volume->convertToFloat();
        rangeTransformation->transformRange(volumeDataInFloatFormat.get(), volume->getProperties().getVolumeVoxelCount());

        Vec3ui outputVolumeResolution = getOrderDependentResolution(volume->getProperties().getVolumeResolution(), options->getOrientation());

        writeHeader(outfile, options->getVoxelType(), volume->getProperties(), outputVolumeResolution, rangeTransformation);

        if(options->getVoxelType() == IO_VOXEL_TYPE_FLOAT_32)
        {
            writeSlicewise(outfile, outputVolumeResolution, volume, options->getOrientation());
        } else {
            writePixelwise(outfile, outputVolumeResolution, volume, options, volumeDataInFloatFormat);
        }

        delete rangeTransformation;

        if( !outfile.good() )
        {
            throw Exception((boost::format("Error encountered while writing to output file %1% !") % outputVolumeFileName).str());
        }
        outfile.close();
    }

    void VolumeFileFormatWriter::writePixelwise(std::ofstream &outfile, Vec3ui &outputVolumeResolution, Volume* volume, const OutputParameterSet* options, std::unique_ptr<float> &volumeDataInFloatFormat)
    {
        unsigned int orderDependentIndices[3];
        fillOrderDependentIndices(orderDependentIndices, options->getOrientation());

        for( unsigned int i = 0; i < outputVolumeResolution.z; ++i )
        {
            for( unsigned int j = 0; j < outputVolumeResolution.y; ++j )
            {
                for( unsigned int k = 0; k < outputVolumeResolution.x; ++k )
                {
                    Vec3ui coords;
                    coords[orderDependentIndices[0]] = i;
                    coords[orderDependentIndices[1]] = j;
                    coords[orderDependentIndices[2]] = k;
                    writeValue(outfile, volumeDataInFloatFormat.get()[volume->getVoxelIndex(coords)], options->getVoxelType());
                }
            }
        }
    }

    void VolumeFileFormatWriter::writeSlicewise(std::ofstream &output, Vec3ui &outputVolumeResolution, Volume* volume, CoordinateOrder orientation)
    {
        for( unsigned int i = 0; i < outputVolumeResolution.z; ++i )
        {
            auto tempSlice = volume->extractSlice(i, orientation);
            writeSlice(output, tempSlice);
            delete tempSlice;
        }
    }

    void VolumeFileFormatWriter::writeValue(std::ofstream &output, float value, VoxelValueType valueType)
    {
        short shortValue;
        unsigned short ushortValue;
        unsigned char ucharValue;
        switch( valueType )
        {
        case IO_VOXEL_TYPE_FLOAT_32:
            output.write((const char*)&value, sizeof(float));
            break;
        case IO_VOXEL_TYPE_SIGNED_16:
            shortValue = (short)value;
            output.write((const char*)&shortValue, sizeof(short));
            break;
        case IO_VOXEL_TYPE_UNSIGNED_16:
            ushortValue = (unsigned short)value;
            output.write((const char*)&ushortValue, sizeof(unsigned short));
            break;
        case IO_VOXEL_TYPE_UNSIGNED_8:
            ucharValue = (unsigned char)value;
            output.write((const char*)&ucharValue, sizeof(unsigned char));
            break;
        default:
            throw Exception("Unknown VoxelValueType given!");
        }
    }

    void VolumeFileFormatWriter::writeSlice(std::ofstream &output, Image *image)
    {
        output.write( (const char*)image->getData(), image->getPixelCount() * sizeof(float) );
    }

    RangeTransformation* VolumeFileFormatWriter::createRangeTransformation(Volume* volume, const OutputParameterSet* options)
    {
        VolumeStatistics statistics;
        if( options->getInvert() )
        {
            switch( options->getVoxelType() )
            {
            case IO_VOXEL_TYPE_UNSIGNED_8:
                return new LinearRangeTransformation(volume, 255.0f, 0.0f);
            case IO_VOXEL_TYPE_UNSIGNED_16:
                return new LinearRangeTransformation(volume, 65535.0f, 0.0f);
            case IO_VOXEL_TYPE_SIGNED_16:
                return new LinearRangeTransformation(volume, 32767.0f, -32768.0f);
            case IO_VOXEL_TYPE_FLOAT_32:
                statistics = VolumeStatistics::compute(volume);
                return new LinearRangeTransformation(volume, statistics.getMax(), statistics.getMin());
            default:
                throw Exception("range transformation for unknown file mode requested");
                break;
            }
        } else
        {
            switch( options->getVoxelType() )
            {
            case IO_VOXEL_TYPE_UNSIGNED_8:
                return new LinearRangeTransformation(volume, 0.0f, 255.0f);
            case IO_VOXEL_TYPE_UNSIGNED_16:
                return new LinearRangeTransformation(volume, 0.0f, 65535.0f);
            case IO_VOXEL_TYPE_SIGNED_16:
                return new LinearRangeTransformation(volume, -32768.0f, 32767.0f);
            case IO_VOXEL_TYPE_FLOAT_32:
                return new IdentityRangeTransformation(volume);
            default:
                throw Exception("range transformation for unknown file mode requested");
                break;
            }
        }
    }

}