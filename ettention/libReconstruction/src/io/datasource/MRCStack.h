#pragma once
#include "io/datasource/ImageStackDatasource.h"
#include "io/datasource/MRCHeader.h"
#include "framework/Logger.h"
#include "model/geometry/SingleAxisTiltRotator.h"
#include "io/datasource/SingleTiltDatasource.h"

namespace ettention
{
    class MRCStack : public ImageStackDatasource, public SingleTiltDatasource
    {
    protected:
        Image* loadProjectionImage(const HyperStackIndex& index) override;

    public:
        MRCStack();
        MRCStack(const boost::filesystem::path& stackFilePath, bool logaritmizeData);
        MRCStack(const boost::filesystem::path& stackFilePath, const boost::filesystem::path& tiltFilePath, bool logaritmizeData);
        ~MRCStack();

        Vec2ui getResolution() const override;
        ScannerGeometry* createScannerGeometry(const HyperStackIndex& index, AlgebraicParameterSet* paramSet) override;
        bool canHandleResource(AlgebraicParameterSet* paramSet) override;
        void openResource(AlgebraicParameterSet* paramSet) override;
        ImageStackDatasource* instantiate(AlgebraicParameterSet* paramSet) override;
        void writeToLog() override;
        unsigned int getNumberOfProjections() const override;
        const char* getName() const override;
        HyperStackIndex firstIndex() const override;
        HyperStackIndex lastIndex() const override;

    protected:
        std::ifstream file;
        float dataMin;
        float dataMax;
        bool logaritmizeData;

    private:
        SingleAxisTiltRotator satRotator;
        ScannerGeometry* baseScannerGeometry;
        boost::filesystem::path stackFilePath;
        MRCHeader mrcHeader;
        Vec2ui resolution;
        unsigned int numberOfProjections;

        void init();
        void setEmptyTiltAngles();
        void readImageData(float* dest, unsigned int projectionIndex);
        ScannerGeometry* createTiltGeometry(float tiltAngleInDegree, AlgebraicParameterSet* paramSet) override;

        template<typename _T>
        void readSlice(float* dest, unsigned int projectionIndex, float minValue);

        template<typename _T>
        float convertValue(_T rawValue, float minValue) const;

        template<typename _T>
        void getDataRange();
    };
}