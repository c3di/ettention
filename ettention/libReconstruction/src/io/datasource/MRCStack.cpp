#include "stdafx.h"
#include "MRCStack.h"
#include <boost/format.hpp>
#include <boost/numeric/conversion/bounds.hpp>
#include "framework/error/Exception.h"
#include "model/image/Image.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/InputParameterSet.h"
#include "model/geometry/ParallelScannerGeometry.h"

namespace ettention
{
    MRCStack::MRCStack()
        : baseScannerGeometry(nullptr)
    {
    }

    MRCStack::MRCStack(const boost::filesystem::path& stackFilePath, bool logaritmizeData)
        : stackFilePath(stackFilePath)
        , logaritmizeData(logaritmizeData)
    {
        init();
        setEmptyTiltAngles();
    }

    MRCStack::MRCStack(const boost::filesystem::path& stackFilePath, const boost::filesystem::path& tiltFilePath, bool logaritmizeData)
        : stackFilePath(stackFilePath)
        , logaritmizeData(logaritmizeData)
    {
        init();
        readTiltAngles(tiltFilePath);
    }

    MRCStack::~MRCStack()
    {
        //if (baseScannerGeometry)
            //delete(baseScannerGeometry);
        if(file.good())
        {
            file.close();
        }
    }

    const char* MRCStack::getName() const
    {
        return "MRCStack";
    }

    ettention::HyperStackIndex MRCStack::firstIndex() const
    {
        return HyperStackIndex(0);
    }

    ettention::HyperStackIndex MRCStack::lastIndex() const
    {
        return HyperStackIndex( numberOfProjections - 1 );
    }

    Image* MRCStack::loadProjectionImage(const HyperStackIndex& index)
    {
        if(index.getNumberOfDimensions() != 1)
        {
            throw Exception("MRC datasource only supports indices of dimension one");
        }
        std::vector<float> data(resolution.x * resolution.y);
        readImageData(&data[0], index[0]);
        return new Image(resolution, &data[0]);
    }

    bool MRCStack::canHandleResource(AlgebraicParameterSet* paramSet)
    {
        boost::filesystem::path location = paramSet->get<InputParameterSet>()->getProjectionsPath();

        if(!boost::filesystem::is_regular_file(location))
            return false;

        if(location.extension() != ".mrc" && location.extension() != ".ali")
            return false;

        LOGGER("Using MRCStack to open file " << paramSet->get<InputParameterSet>()->getProjectionsPath() << ".");

        return true;
    }

    void MRCStack::openResource(AlgebraicParameterSet* paramSet)
    {
        stackFilePath = paramSet->get<InputParameterSet>()->getProjectionsPath();
        this->logaritmizeData = paramSet->get<InputParameterSet>()->getLogarithmize();
        init();
        try
        {
            readTiltAngles(paramSet->get<InputParameterSet>()->getTiltAnglesPath().string());
        }
        catch (Exception e)
        {
            throw Exception((boost::format("MRC stack from %1%: ") % stackFilePath).str().c_str());
        }
    }

    ImageStackDatasource* MRCStack::instantiate(AlgebraicParameterSet* paramSet)
    {
        if(paramSet->get<InputParameterSet>()->getTiltAnglesPath() != "")
            return new MRCStack(paramSet->get<InputParameterSet>()->getProjectionsPath().string(), paramSet->get<InputParameterSet>()->getTiltAnglesPath().string(), paramSet->get<InputParameterSet>()->getLogarithmize());
        return new MRCStack(paramSet->get<InputParameterSet>()->getProjectionsPath().string(), paramSet->get<InputParameterSet>()->getLogarithmize());
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
        LOGGER_INFO_FORMATTED("Number of labels:" << mrcHeader.nLabel);
        for(unsigned int i = 0; i < mrcHeader.nLabel; i++)
        {
            LOGGER_INFO_FORMATTED(mrcHeader.labels[i]);
        }
    }

    void MRCStack::readImageData(float* dest, unsigned int projectionIndex)
    {
        switch(mrcHeader.mode)
        {
        case 0:
            this->readSlice<unsigned char>(dest, projectionIndex, -1.0f);
            break;
        case 1:
            this->readSlice<short>(dest, projectionIndex, dataMin - 1.0f);
            break;
        case 2: 
            this->readSlice<float>(dest, projectionIndex, dataMin - 1.0f);
            break;
        case 6:
            this->readSlice<unsigned short>(dest, projectionIndex, -1.0f);
            break;
        default:
            LOGGER_ERROR("Format of the MRC stack is either unknown or not supported");
            break;
        }
    }

    template <typename _T>
    float MRCStack::convertValue(_T rawValue, float minValue) const
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

    template <typename _T>
    void MRCStack::readSlice(float* dest, unsigned int projectionIndex, float minValue)
    {
        auto pixels = resolution.x * resolution.y;
        _T* data = new _T[pixels];
        file.seekg(sizeof(MRCHeader) + mrcHeader.extra + (projectionIndex * pixels) * sizeof(_T));
        file.read((char*)data, pixels * sizeof(_T));
        for(int i = 0; i < mrcHeader.ny; i++)
        {
            for(int j = 0; j < mrcHeader.nx; j++)
            {
                float value = convertValue(data[i * mrcHeader.nx + j], minValue);
                dest[i*mrcHeader.nx + j] = value;
            }
        }
        delete[] data;
    }

    template<typename _T>
    void MRCStack::getDataRange()
    {
        dataMax = -std::numeric_limits<float>::infinity();
        dataMin = std::numeric_limits<float>::infinity();
        std::size_t voxelCount = (std::size_t)mrcHeader.ny * (std::size_t)mrcHeader.nx * (std::size_t)mrcHeader.nz;
        for(std::size_t i = 0; i < voxelCount; ++i)
        {
            _T value;
            file.read((char*)&value, sizeof(_T));
            if(!file.good())
            {
                ETTENTION_THROW_EXCEPTION("Error reading " << voxelCount << " voxel value(s) from " << stackFilePath << "!");
            }
            dataMax = std::max((float)value, dataMax);
            dataMin = std::min((float)value, dataMin);
        }
    }

    void MRCStack::init()
    {
        file.open(stackFilePath.string().c_str(), std::ios::binary);
        if(!file.good())
        {
            throw Exception((boost::format("Could not open MRC stack %1%") % stackFilePath).str());
        }
        file.read((char*)&mrcHeader, sizeof(MRCHeader));
        file.ignore(mrcHeader.extra);

        numberOfProjections = mrcHeader.nz;

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

        baseScannerGeometry = new ParallelScannerGeometry(resolution);
        baseScannerGeometry->set(source, detector, Vec3f(1.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f));
        geoCreator.setBaseScannerGeometry(baseScannerGeometry);
    }

    void MRCStack::setEmptyTiltAngles()
    {
        tiltAnglesInDegree.resize(getNumberOfProjections());
        std::fill(tiltAnglesInDegree.begin(), tiltAnglesInDegree.end(), 0.0f);
    }

    unsigned int MRCStack::getNumberOfProjections() const
    {
        return numberOfProjections;
    }

    Vec2ui MRCStack::getResolution() const
    {
        return resolution;
    }

    ScannerGeometry* MRCStack::getScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet)
    {
        if(index.getNumberOfDimensions() != 1)
        {
            throw Exception("MRC datasource only supports indices of dimension one");
        }
        return geoCreator.getRotatedScannerGeometry(this->tiltAnglesInDegree[index[0]], paramSet->get<InputParameterSet>()->getXAxisTilt());
    }

    ScannerGeometry* MRCStack::getTiltGeometry(float tiltAngleInDegree, AlgebraicParameterSet* paramSet)
    {
        return geoCreator.getRotatedScannerGeometry(tiltAngleInDegree, paramSet->get<InputParameterSet>()->getXAxisTilt());
    }
}