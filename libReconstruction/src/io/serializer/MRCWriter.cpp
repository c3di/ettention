#include "stdafx.h"

#include "framework/error/Exception.h"
#include "io/serializer/MRCWriter.h"
#include "io/datasource/MRCHeader.h"

namespace ettention
{
    MRCWriter::MRCWriter()
    {
    }

    MRCWriter::~MRCWriter()
    {
    }

    bool MRCWriter::canGenerateVoxelType(OutputFormatParameterSet::VoxelValueType mode)
    {
        if(mode == OutputFormatParameterSet::VoxelValueType::UNSIGNED_8)
            return true;
        if(mode == OutputFormatParameterSet::VoxelValueType::UNSIGNED_16)
            return true;
        if(mode == OutputFormatParameterSet::VoxelValueType::FLOAT_32)
            return true;
        return false;
    }

    bool MRCWriter::canGenerateRotation(OutputFormatParameterSet::CoordinateOrder rotation)
    {
        return true;
    }

    std::string MRCWriter::getFileExtension()
    {
        return std::string(".mrc");
    }

    void MRCWriter::write(Volume* volume, boost::filesystem::path outputVolumeFileName, const OutputFormatParameterSet* options)
    {
        this->outputVolumeFileName = outputVolumeFileName;

        RangeTransformation* rangeTransformation = createRangeTransformation(volume, options);

        outfile.open(outputVolumeFileName.string(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if(!outfile.good())
            throw Exception((boost::format("could not open output file %1%") % outputVolumeFileName).str());

        writeMRCHeader(options->VoxelType(), volume->Properties(), options->OutputVolumeRotation(), rangeTransformation);

        auto volumeDataInFloatFormat = volume->ConvertToFloat();
        rangeTransformation->transformRange(volumeDataInFloatFormat.get(), volume->Properties().GetVolumeVoxelCount());

        writeBuffer = NULL;
        float* volumeDataPtr = volumeDataInFloatFormat.get();

        switch(options->OutputVolumeRotation())
        {
        case OutputFormatParameterSet::ORDER_XYZ:
            for(unsigned int z = 0; z < volume->Properties().GetVolumeResolution().z; z++)
            {
                for(unsigned int y = 0; y < volume->Properties().GetVolumeResolution().y; y++)
                {
                    float* pointerToDataRow = volumeDataPtr + volume->GetVoxelIndex(Vec3ui(0, y, z));
                    writeMRCSlice(pointerToDataRow, volume->Properties().GetVolumeResolution().x, options->VoxelType());
                }
            }
            break;
        case OutputFormatParameterSet::ORDER_XZY:
            for(unsigned int y = 0; y < volume->Properties().GetVolumeResolution().y; y++)
            {
                for(unsigned int z = 0; z < volume->Properties().GetVolumeResolution().z; z++)
                {
                    float* pointerToDataRow = volumeDataPtr + volume->GetVoxelIndex(Vec3ui(0, y, z));
                    writeMRCSlice(pointerToDataRow, volume->Properties().GetVolumeResolution().x, options->VoxelType());
                }
            }
            break;
        default:
            throw Exception("Illegal output coordinate order!");
        }

        outfile.close();
        delete rangeTransformation;
    }

    void MRCWriter::writeMRCHeader(OutputFormatParameterSet::VoxelValueType fileMode, VolumeProperties& volumeProperties, OutputFormatParameterSet::CoordinateOrder rotation, RangeTransformation* rangeTransformation)
    {
        MRCHeader header;
        memset(&header, '\0', sizeof(MRCHeader));

        header.mode = fileMode;

        switch(rotation)
        {
        case OutputFormatParameterSet::ORDER_XZY:
            header.nx = volumeProperties.GetVolumeResolution().x;
            header.ny = volumeProperties.GetVolumeResolution().z;
            header.nz = volumeProperties.GetVolumeResolution().y;

            header.mx = volumeProperties.GetVolumeResolution().x;
            header.my = volumeProperties.GetVolumeResolution().z;
            header.mz = volumeProperties.GetVolumeResolution().y;

            header.cellDimX = (float)volumeProperties.GetVolumeResolution().x;
            header.cellDimY = (float)volumeProperties.GetVolumeResolution().z;
            header.cellDimZ = (float)volumeProperties.GetVolumeResolution().y;
            break;

        case OutputFormatParameterSet::ORDER_XYZ:
            header.nx = volumeProperties.GetVolumeResolution().x;
            header.ny = volumeProperties.GetVolumeResolution().y;
            header.nz = volumeProperties.GetVolumeResolution().z;

            header.mx = volumeProperties.GetVolumeResolution().x;
            header.my = volumeProperties.GetVolumeResolution().y;
            header.mz = volumeProperties.GetVolumeResolution().z;

            header.cellDimX = (float)volumeProperties.GetVolumeResolution().x;
            header.cellDimY = (float)volumeProperties.GetVolumeResolution().y;
            header.cellDimZ = (float)volumeProperties.GetVolumeResolution().z;
            break;

        default:
            throw Exception("illegal coordinate ordering");
        }

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
        if(!outfile.good())
            throw Exception((boost::format("error writing to output file %1%") % outputVolumeFileName).str());
    }

    void MRCWriter::writeMRCSlice(float* slice, size_t length, OutputFormatParameterSet::VoxelValueType fileMode)
    {
        switch(fileMode)
        {
        case OutputFormatParameterSet::UNSIGNED_8:
            writeMRCSliceInSpecificFormat<unsigned char>(slice, length);
            break;

        case OutputFormatParameterSet::SIGNED_16:
            writeMRCSliceInSpecificFormat<signed short>(slice, length);
            break;

        case OutputFormatParameterSet::FLOAT_32:
            outfile.write((const char*)slice, length*sizeof(float));
            if(!outfile.good())
                throw Exception((boost::format("error writing to output file %1%") % outputVolumeFileName).str());
            break;

        case OutputFormatParameterSet::UNSIGNED_16:
            writeMRCSliceInSpecificFormat<unsigned short>(slice, length);
            break;

        default:
            throw Exception("Format of the MRC stack is either unknown or not supported");
            break;
        }
    }

    template <typename T>
    void MRCWriter::writeMRCSliceInSpecificFormat(float* slice, size_t length)
    {
        if(writeBuffer == NULL)
            writeBuffer = new T[length];
        for(size_t i = 0; i < length; i++)
            ((T*)writeBuffer)[i] = (T)slice[i];
        outfile.write((const char*)writeBuffer, length*sizeof(T));
        if(!outfile.good())
            throw Exception((boost::format("error writing to output file %1%") % outputVolumeFileName).str());

        T* typedWB = reinterpret_cast<T*>(writeBuffer);
        delete[] typedWB;
        writeBuffer = NULL;
    }
}
