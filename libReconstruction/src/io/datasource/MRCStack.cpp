#include "stdafx.h"
#include "MRCStack.h"
#include <boost/format.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include "framework/error/Exception.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"
#include "setup/ParameterSet/IOParameterSet.h"

namespace ettention
{
    MRCStack::MRCStack()
    {
        file = NULL;
    }

    MRCStack::MRCStack(const boost::filesystem::path& stackFilePath, bool logaritmizeData)
        : stackFilePath(stackFilePath)
        , logaritmizeData(logaritmizeData)
    {
        this->Init();
        this->SetEmptyTiltAngles();
    }

    MRCStack::MRCStack(const boost::filesystem::path& stackFilePath, const boost::filesystem::path& tiltFilePath, bool logaritmizeData)
        : stackFilePath(stackFilePath)
        , logaritmizeData(logaritmizeData)
    {
        this->Init();
        this->ReadTiltAngles(tiltFilePath);
    }

    MRCStack::~MRCStack()
    {
        if(file != NULL)
            fclose(file);
    }

    Image* MRCStack::LoadProjectionImage(const HyperStackIndex& index)
    {
        if(index.getNumberOfDimensions() != 1)
        {
            throw Exception("MRC datasource only supports indices of dimension one");
        }
        std::vector<float> data(resolution.x * resolution.y);
        this->ReadImageData(&data[0], index[0]);
        return new Image(resolution, &data[0]);
    }

    bool MRCStack::canHandleResource(AlgebraicParameterSet* paramSet)
    {
        boost::filesystem::path location = paramSet->get<IOParameterSet>()->InputStackFileName();

        if(!boost::filesystem::is_regular_file(location))
            return false;

        if(location.extension() != ".mrc" && location.extension() != ".ali")
            return false;

        LOGGER("using MRCStack to open file " << paramSet->get<IOParameterSet>()->InputStackFileName());

        return true;
    }

    void MRCStack::openResource(AlgebraicParameterSet* paramSet)
    {
        stackFilePath = paramSet->get<IOParameterSet>()->InputStackFileName();
        this->logaritmizeData = paramSet->get<IOParameterSet>()->LogaritmizeData();
        Init();
        ReadTiltAngles(paramSet->get<IOParameterSet>()->TiltAnglesFileName().string());
    }

    ImageStackDatasource* MRCStack::instantiate(AlgebraicParameterSet* paramSet)
    {
        if(paramSet->get<IOParameterSet>()->TiltAnglesFileName() != "")
            return new MRCStack(paramSet->get<IOParameterSet>()->InputStackFileName().string(), paramSet->get<IOParameterSet>()->TiltAnglesFileName().string(), paramSet->get<IOParameterSet>()->LogaritmizeData());
        return new MRCStack(paramSet->get<IOParameterSet>()->InputStackFileName().string(), paramSet->get<IOParameterSet>()->LogaritmizeData());
    }

    void MRCStack::writeToLog()
    {
        LOGGER("Datasource type: MRCStack");
        LOGGER_INFO_FORMATTED("Number of columns, rows, sections" << mrcHeader.nx << ", " << mrcHeader.ny << ", " << mrcHeader.nz);
        LOGGER_INFO_FORMATTED("Map mode" << mrcHeader.mode);
        LOGGER_INFO_FORMATTED("Start columns, rows, sections" << mrcHeader.nxStart << ", " << mrcHeader.nyStart << ", " << mrcHeader.nzStart);
        LOGGER_INFO_FORMATTED("Grid size in x, y, z" << mrcHeader.mx << ", " << mrcHeader.my << ", " << mrcHeader.mz);
        LOGGER_INFO_FORMATTED("Cell dimensions in x, y, z" << mrcHeader.cellDimX << ", " << mrcHeader.cellDimY << ", " << mrcHeader.cellDimZ);
        LOGGER_INFO_FORMATTED("Cell angles in x, y, z (degrees)" << mrcHeader.cellAngleX << ", " << mrcHeader.cellAngleY << ", " << mrcHeader.cellAngleZ);
        LOGGER_INFO_FORMATTED("Axis corresponding to columns, rows and sections (1,2,3 for X,Y,Z)" << mrcHeader.mapC << ", " << mrcHeader.mapR << ", " << mrcHeader.mapS);
        LOGGER_INFO_FORMATTED("Origin on x, y, z" << mrcHeader.originX << ", " << mrcHeader.originY << ", " << mrcHeader.originZ);
        LOGGER_INFO_FORMATTED("Minimum density" << mrcHeader.dMin);
        LOGGER_INFO_FORMATTED("Maximum density" << mrcHeader.dMax);
        LOGGER_INFO_FORMATTED("Mean density" << mrcHeader.dMean);
        LOGGER_INFO_FORMATTED("Tilt angles - original" << mrcHeader.tiltangles[0] << ", " << mrcHeader.tiltangles[1] << ", " << mrcHeader.tiltangles[2]);
        LOGGER_INFO_FORMATTED("Tilt angles - current" << mrcHeader.tiltangles[3] << ", " << mrcHeader.tiltangles[4] << ", " << mrcHeader.tiltangles[5]);
        LOGGER_INFO_FORMATTED("Space group" << mrcHeader.ISPG);
        LOGGER_INFO_FORMATTED("Number of bytes used for symmetry data" << mrcHeader.NSYMBT);
        LOGGER_INFO_FORMATTED("Number of bytes in extended header" << mrcHeader.extra);
        LOGGER_INFO_FORMATTED("Creator ID" << mrcHeader.creatorID);
        LOGGER_INFO_FORMATTED("ID type" << mrcHeader.idtype);
        LOGGER_INFO_FORMATTED("Lens" << mrcHeader.lens);
        LOGGER_INFO_FORMATTED(mrcHeader.nLabel << "labels:");
        for(unsigned int i = 0; i < mrcHeader.nLabel; i++)
        {
            LOGGER_INFO_FORMATTED(mrcHeader.labels[i]);
        }
    }

    void MRCStack::ReadImageData(float* dest, unsigned int projectionIndex) const
    {
        switch(mrcHeader.mode)
        {
        case 0:
            this->ReadSlice<unsigned char>(dest, projectionIndex, -1.0f);
            break;
        case 1:
            this->ReadSlice<short>(dest, projectionIndex, dataMin - 1.0f);
            break;
        case 2: 
            this->ReadSlice<float>(dest, projectionIndex, dataMin - 1.0f);
            break;
        case 6:
            this->ReadSlice<unsigned short>(dest, projectionIndex, -1.0f);
            break;
        default:
            LOGGER_ERROR("Format of the MRC stack is either unknown or not supported");
            break;
        }
    }

    template <typename T>
    float MRCStack::ConvertValue(T rawValue, float minValue) const
    {
        float rawFloat = (float)rawValue;
        if(logaritmizeData)
        {
            return logf(dataMax / (rawFloat - minValue));
        }
        else
        {
            return rawFloat;
        }
    }

    template <typename T>
    void MRCStack::ReadSlice(float* dest, unsigned int projectionIndex, float minValue) const
    {
        auto pixels = resolution.x * resolution.y;
        T* data = new T[pixels];
        fseek(file, sizeof(MRCHeader)+mrcHeader.extra + (projectionIndex * pixels) * sizeof(T), SEEK_SET);
        fread(data, pixels * sizeof(T), 1, file);
        for(int i = 0; i < mrcHeader.ny; i++)
        {
            for(int j = 0; j < mrcHeader.nx; j++)
            {
                float value = this->ConvertValue(data[i * mrcHeader.nx + j], minValue);
                dest[i*mrcHeader.nx + j] = value;
            }
        }
        delete[] data;
    }

    template<typename T>
    void MRCStack::getDataRange()
    {
        std::vector<T> buffer(4096);
        dataMax = boost::numeric::bounds<float>::lowest();
        dataMin = boost::numeric::bounds<float>::highest();
        unsigned int remainingVoxel = mrcHeader.ny * mrcHeader.nx * mrcHeader.nz;
        while(remainingVoxel > 0)
        {
            size_t elementsRead = fread(&buffer[0], sizeof(T), buffer.size(), file);
            remainingVoxel -= (unsigned int)elementsRead;
            for(unsigned int i = 0; i < elementsRead; i++)
            {
                dataMax = std::max((float)buffer[i], dataMax);
                dataMin = std::min((float)buffer[i], dataMin);
            }
        }
    }

    void MRCStack::Init()
    {
        file = fopen(stackFilePath.string().c_str(), "rb");
        if(!file)
        {
            throw Exception((boost::format("Could not open MRC stack %1%") % stackFilePath).str());
        }
        fread(&mrcHeader, sizeof(MRCHeader), 1, file);

        numberOfProjections = mrcHeader.nz;
        fseek(file, sizeof(MRCHeader) + mrcHeader.extra, SEEK_SET);

        switch(mrcHeader.mode)
        {
        case 0:
            this->getDataRange<unsigned char>();
            break;
        case 1:
            this->getDataRange<signed short>();
            break;
        case 2:
            this->getDataRange<float>();
            break;
        case 6:
            this->getDataRange<unsigned short>();
            break;
        default:
            throw Exception((boost::format("Format of the MRC stack is either unknown or not supported (mode %1%)") % mrcHeader.mode).str());
        }

        if(mrcHeader.mode == 1 || mrcHeader.mode == 2)
        {
            dataMax = dataMax - dataMin + 1.0f;
        }
        else if(mrcHeader.mode == 0 || mrcHeader.mode == 6)
        {
            dataMax = dataMax + 1.0f;
        }

        resolution = Vec2ui(mrcHeader.nx, mrcHeader.ny);

        Vec3f source = Vec3f((float)mrcHeader.nx, (float)mrcHeader.ny, (float)mrcHeader.nx);
        source.x *= -0.5f;
        source.y *= -0.5f;
        source.z *= 1.0f;

        Vec3f detector = Vec3f((float)mrcHeader.nx, (float)mrcHeader.ny, (float)mrcHeader.nx);
        detector.x *= -0.5f;
        detector.y *= -0.5f;
        detector.z *= -1.0f;

        ScannerGeometry baseScannerGeometry(ScannerGeometry::PROJECTION_PARALLEL, resolution);
        baseScannerGeometry.set(source, detector, Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f));
        geoCreator.SetBaseScannerGeometry(baseScannerGeometry);
    }

    void MRCStack::SetEmptyTiltAngles()
    {
        tiltAnglesInDegree.resize(getNumberOfProjections());
        std::fill(tiltAnglesInDegree.begin(), tiltAnglesInDegree.end(), 0.0f);
    }

    unsigned int MRCStack::getNumberOfProjections()
    {
        return numberOfProjections;
    }

    Vec2ui MRCStack::getResolution() const
    {
        return resolution;
    }

    ScannerGeometry MRCStack::getScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet)
    {
        if(index.getNumberOfDimensions() != 1)
        {
            throw Exception("MRC datasource only supports indices of dimension one");
        }
        return geoCreator.GetRotatedScannerGeometry(this->tiltAnglesInDegree[index[0]], paramSet->get<IOParameterSet>()->XAxisTilt());
    }

    ScannerGeometry::ProjectionType MRCStack::getProjectionType() const
    {
        return ScannerGeometry::PROJECTION_PARALLEL;
    }

    ScannerGeometry MRCStack::getTiltGeometry(float tiltAngleInDegree, float xAxisTiltAngleInDegree)
    {
        return geoCreator.GetRotatedScannerGeometry(tiltAngleInDegree, xAxisTiltAngleInDegree);
    }
}