#pragma once
#include "model/geometry/ScannerGeometry.h"

namespace ettention
{
    class SingleAxisTiltRotator
    {
    private:
        ScannerGeometry baseScannerGeometry;

        void Rotate(Vec3f &coord, float tiltAngleInRadians, float xAxisTiltAngleInRadians);

    public:
        SingleAxisTiltRotator();
        ~SingleAxisTiltRotator();

        void SetBaseScannerGeometry(const ScannerGeometry& baseScannerGeometry);
        const ScannerGeometry& GetBaseScannerGeometry() const;
        ScannerGeometry& GetBaseScannerGeometry();
        ScannerGeometry GetRotatedScannerGeometry(float tiltAngleInDegree, float xAxisTiltAngleInDegree = 0.0f);
    };
}