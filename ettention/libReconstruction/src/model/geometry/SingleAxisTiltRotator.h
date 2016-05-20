#pragma once
#include "model/geometry/ScannerGeometry.h"

namespace ettention
{
    class SingleAxisTiltRotator
    {
    private:
        ScannerGeometry* baseScannerGeometry;
        bool wasBaseScannerGeometrySet;

        void doRotate(Vec3f &coord, float tiltAngleInRadians, float xAxisTiltAngleInRadians);

    public:
        SingleAxisTiltRotator();
        ~SingleAxisTiltRotator();

        void setBaseScannerGeometry( ScannerGeometry* baseScannerGeometry);
        ScannerGeometry* getBaseScannerGeometry() const;
        ScannerGeometry* getBaseScannerGeometry();
        ScannerGeometry* createRotatedScannerGeometry(float tiltAngleInDegree, float xAxisTiltAngleInDegree = 0.0f);
    };
}