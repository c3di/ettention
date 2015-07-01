#pragma once
#include "io/datasource/ImageStackDatasource.h"
#include "io/datasource/MRCHeader.h"
#include "framework/Logger.h"
#include "setup/ParameterSet/IOParameterSet.h"
#include "model/geometry/SingleAxisTiltRotator.h"
#include "io/datasource/SingleTiltDatasource.h"

namespace ettention
{
    class MRCStack : public ImageStackDatasource, public SingleTiltDatasource
    {
    private:
        SingleAxisTiltRotator geoCreator;
        boost::filesystem::path stackFilePath;
        MRCHeader mrcHeader;
        Vec2ui resolution;
        unsigned int numberOfProjections;
        //std::vector<float> tiltAnglesInDegree;

        void Init();
        void SetEmptyTiltAngles();
        void ReadImageData(float* dest, unsigned int projectionIndex) const;
        ScannerGeometry getTiltGeometry(float tiltAngleInDegree, float xAxisTiltAngleInDegree) override;

        template<typename T>
        void ReadSlice(float* dest, unsigned int projectionIndex, float minValue) const;

        template<typename T>
        float ConvertValue(T rawValue, float minValue) const;

        template<typename T>
        void getDataRange();


    protected:
        Image* LoadProjectionImage(const HyperStackIndex& index) override;

    public:
        MRCStack();
        MRCStack(const boost::filesystem::path& stackFilePath, bool logaritmizeData);
        MRCStack(const boost::filesystem::path& stackFilePath, const boost::filesystem::path& tiltFilePath, bool logaritmizeData);
        ~MRCStack();

        Vec2ui getResolution() const override;
        ScannerGeometry getScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet) override;
        ScannerGeometry::ProjectionType getProjectionType() const override;

        bool canHandleResource(AlgebraicParameterSet* paramSet) override;
        void openResource(AlgebraicParameterSet* paramSet) override;
        ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet) override;
        void writeToLog() override;
        unsigned int getNumberOfProjections() override;

    protected:
        FILE *file;
        float dataMin;
        float dataMax;        
        bool logaritmizeData;
    };
}